
#include "ksem.h"		// ksem.cpp

#include "kernel.h"		// Kernel::allSems
#include "semlist.h"	// new ListSemaphores()
#include "defs.h"		// min
#include "schedule.h"	// Scheduler::put
#include "pcb.h"		// PCB::running
#include "thread.h"		// dispatch()

KernelSem::KernelSem(int init, Semaphore *mySemaphore) {
	value = init;
	this->mySemaphore = mySemaphore;
	blocked = new ListPCB();

	if (Kernel::allSems == 0) // happens when someone tries to create global Semaphore and Kernel::initialize has not been called yet
		Kernel::allSems = new ListSemaphores();
	Kernel::allSems->append(this);
}

KernelSem::~KernelSem() {
	blocked->moveEverythingToScheduler();
	delete blocked;
	if(Kernel::allSems)
		Kernel::allSems->remove(this);
}

void KernelSem::block() {
	PCB::running->state = PCB::BLOCKED;
	blocked->append(PCB::running);
	dispatch();
}

void KernelSem::unblock(int howMany) {
	if(howMany <= 0)
		return;
	for(int i = 0; i < howMany; i++) {
		PCB *ready = blocked->popFirst();
		if(ready) {
			ready->valueToReturnInSemWait = 1; // should also work without this because 1 is default value
			ready->setState(PCB::READY);
			Scheduler::put(ready);
		}
	}
}

int KernelSem::wait(Time maxTimeToWait) {
	PCB::running->semSlice = maxTimeToWait;
	if(--value < 0)
		block();

	return PCB::running->valueToReturnInSemWait;
}

int KernelSem::signal(int n) {
	if(n < 0)
		return n;

	int ret = 0;

	if(n == 0) { // normal signal
		if(value++ < 0)
			unblock(); // unblock(1)
		return ret;
	}else {
		value += n; // Value of semaphore increments for given n
		ret = min(blocked->getLength(), n); // if n > 0 and n > length(blocked) then return length(blocked), if n > 0 and n < length(blocked) then return n
		unblock(ret);
		return ret;
	}
}
