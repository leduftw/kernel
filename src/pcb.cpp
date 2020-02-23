
#include <dos.h>		// FP_SEG, FP_OFF
#include <stdlib.h>		// new? apparently not

#include "pcb.h"		// pcb.cpp

#include "kernel.h"		// allPCBs->append(this)
#include "schedule.h"	// Scheduler::put
#include "defs.h"		// lock, unlock

ID PCB::LAST_ID = 0;
volatile int PCB::counter = 0;

PCB* PCB::main = 0;
PCB* PCB::idle = 0;
PCB* PCB::running = 0;

PCB::PCB(StackSize sz, Time slice, Thread *mine) {
	id = ++LAST_ID;

	myThread = mine;
	timeSlice = slice;
	stackSize = (sz > 65536 ? 65536 : sz) / sizeof(unsigned);

	started = 0; // flag for idle thread

	// On 8086, maximum size for allocation is 64KB, if we tried more it wouldn't work
	stack = new unsigned[stackSize];

	ss = sp = bp = 0;

	valueToReturnInSemWait = 1;
	semSlice = 0;

	state = CREATED;

	if (Kernel::allPCBs == 0) // happens when someone tries to create global Thread and Kernel::initialize has not been called yet
		Kernel::allPCBs = new ListPCB();
	Kernel::allPCBs->append(this);

	threadsWaitingForThisToFinish = new ListPCB();
}

PCB::~PCB() {
	delete threadsWaitingForThisToFinish;
	if(Kernel::allPCBs)
		Kernel::allPCBs->remove(this);
	delete[] stack;
}

void PCB::createContext() {
	// slajd 4, vezbe 2
	stack[stackSize - 1] = 0x200; // PSW (I flag is set to 1)

#ifndef BCC_BLOCK_IGNORE
	stack[stackSize - 2] = FP_SEG(&wrapper); // CS
	stack[stackSize - 3] = FP_OFF(&wrapper); // IP

	// ax, bx, cx, dx, es, ds, si, di, bp
	sp = FP_OFF(stack + stackSize - 12); // now stack pointer points to bp (last register that was pushed)
	ss = FP_SEG(stack + stackSize - 12);
	bp = FP_OFF(stack + stackSize - 12);
#endif

	// bp = sp;
}

void PCB::start() {
	if(state == CREATED) { // prohibiting multiple start() calls for same thread (it will never be in PCB::CREATED state again)
		state = READY;
		createContext();
		Scheduler::put(this);
	}

	if(state == IDLE && !started) { // with another flag we solve problem for multiple start() calls on idle thread
		started = 1;
		createContext(); // Idle thread should by no means finish in scheduler!!!
	}
}

void PCB::wrapper() {
	PCB::running->myThread->run();
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	// All threads which were waiting for this to finish are now ready and should be moved from waiting list to scheduler
	PCB::running->threadsWaitingForThisToFinish->moveEverythingToScheduler();
	PCB::running->state = FINISHED;
	dispatch();
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

void PCB::waitToComplete() {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	// If it's in state ready, waiting or blocked that means that it has not finished yet
	// PCB::CREATED shouldn't be checked since thread that called this method shouldn't rely
	// on assumption that some other will call start() for this thread
	if((state == READY || state == WAITING || state == BLOCKED) && this != PCB::running) { // this->state!!!
		PCB::running->state = WAITING;
		threadsWaitingForThisToFinish->append(PCB::running); // list for THIS object
		dispatch();
	}
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

PCB* PCB::getPCBById(ID id) {
	for(Kernel::allPCBs->toFirst(); Kernel::allPCBs->hasCurr(); Kernel::allPCBs->next())
		if(Kernel::allPCBs->getCurrPCB()->getId() == id)
			return Kernel::allPCBs->getCurrPCB();
	return 0;
}


