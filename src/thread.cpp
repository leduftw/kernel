
#include "thread.h"		// thread.cpp

#include "kernel.h"		// Kernel::dispatch()
#include "pcb.h"		// PCB
#include "defs.h"		// lock, unlock

Thread::Thread(StackSize stackSize, Time timeSlice) {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	myPCB = new PCB(stackSize, timeSlice, this);
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

Thread::~Thread() {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	waitToComplete();
	if(myPCB) {
		delete myPCB; // if myPCB == 0, then delete does nothing, else destructor of class PCB ensures that this PCB is removed from allPCBs
		myPCB = 0;
	}
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

void Thread::start() {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	if(myPCB)
		myPCB->start();
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

void Thread::waitToComplete() {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	if(myPCB)
		myPCB->waitToComplete();
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

ID Thread::getId() {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	ID ret = myPCB ? myPCB->getId() : -1;
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
	return ret;
}

ID Thread::getRunningId() {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	ID ret = PCB::getRunningID();
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
	return ret;
}

Thread* Thread::getThreadById(ID id) {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	PCB *pcb = PCB::getPCBById(id);
	Thread *ret = pcb ? pcb->myThread : 0;
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
	return ret;
}

void dispatch() {
	// Lock and unlock are not necessary here because entering interrupt routine automatically does that
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	Kernel::dispatch();
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}
