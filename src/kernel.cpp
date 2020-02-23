
#include <iostream.h>	// cout, endl

#include "kernel.h"    	// kernel.cpp

#include "pcb.h"		// PCB
#include "schedule.h"	// Scheduler::put, Scheduler::get
#include "defs.h"		// lock, unlock

ListPCB* Kernel::allPCBs = 0;
ListSemaphores* Kernel::allSems = 0;

Timer* Kernel::t = 0;
Thread* Kernel::main = 0;
IdleThread* Kernel::idle = 0;

static unsigned tsp = 0, tss = 0, tbp = 0; // only used in Kernel::dispatch

void Kernel::createMainThread() {
	// main thread doesn't need stack allocation
	main = new Thread(defaultStackSize, 1);	// change here timeSlice for the main thread
	main->myPCB->setState(PCB::READY);
	PCB::main = main->myPCB;
	PCB::running = main->myPCB;
	PCB::counter = PCB::running->timeSlice;
}

void Kernel::createIdleThread() {
	idle = new IdleThread(); // timeSlice == 1, but it doesn't matter since on every timer interrupt we check if IDLE is running and if it is, we switch context
	idle->myPCB->setState(PCB::IDLE);
	PCB::idle = idle->myPCB;
	idle->start();
}

void Kernel::initialize() {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	if(allPCBs == 0) // if someone created global Thread first, then list already exist
		allPCBs = new ListPCB();
	if(allSems == 0) // if someone created global Semaphore first, then list already exist
		allSems = new ListSemaphores();

	t = new Timer();
	createIdleThread();
	createMainThread();
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

void Kernel::restore() {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	delete idle; // even though we have in next few lines delete allPCBs, there would still left Thread object
	delete main; // but this will also delete main and idle PCB from allPCBs
	delete t;
	delete allSems;
	delete allPCBs;
	allSems = 0;
	allPCBs = 0;
	// PCB::running->state = PCB::FINISHED; // PCB::running is now hanging pointer and this is not wise
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

// Context switch
void interrupt Kernel::dispatch(...) {
#ifndef BCC_BLOCK_IGNORE
	asm {
		mov tsp, sp
		mov tss, ss
		mov tbp, bp
	}
#endif

	PCB::running->sp = tsp;
	PCB::running->ss = tss;
	PCB::running->bp = tbp;

	if(PCB::running->state == PCB::READY) // there is no way for state to be CREATED so we don't have to include that
		Scheduler::put(PCB::running); // only READY PCBs can be in scheduler
	PCB::running = Scheduler::get();
	if(PCB::running == 0) // if there are no PCB::READY PCBs then idle thread should run
		PCB::running = PCB::idle;

	// cout << "Izabrana nit #" << PCB::getRunningID() << " iz scheduler-a!" << endl;
	// cout sometimes sets I bit to 1, watch out!

	tsp = PCB::running->sp;
	tss = PCB::running->ss;
	tbp = PCB::running->bp;

	PCB::counter = PCB::running->timeSlice;

#ifndef BCC_BLOCK_IGNORE
	asm {
		mov sp, tsp
		mov ss, tss
		mov bp, tbp
	}
#endif
}
