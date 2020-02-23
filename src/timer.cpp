
#include <dos.h>		// getvect, setvect

#include "timer.h"		// timer.cpp

#include "kernel.h"		// Kernel::allSems
#include "schedule.h"	// Scheduler::put
#include "thread.h"		// dispatch()
#include "pcb.h"		// PCB
#include "ksem.h"		// KernelSem *curr

pInterrupt Timer::oldInterrupt = 0;

void tick(); // it will be provided in tests

Timer::Timer() {
// Initializing: setting our interrupt routine in entry reserved for real timer interrupt
#ifndef BCC_BLOCK_IGNORE
	lock;
	oldInterrupt = getvect(8);
	setvect(8, timer);
	unlock;
#endif
}

Timer::~Timer() {
// Restoring: getting back old interrupt routine in entry number 8h
#ifndef BCC_BLOCK_IGNORE
	lock;
	setvect(8, oldInterrupt);
	unlock;
#endif
}

void interrupt Timer::timer(...) {
	oldInterrupt();
	tick();

	//---------- FOR SEMAPHORES ----------//

	for(Kernel::allSems->toFirst(); Kernel::allSems->hasCurr(); Kernel::allSems->next()) { // Going through all semaphores
		KernelSem *currSem = Kernel::allSems->getCurrKSem();
		for(currSem->blocked->toFirst(); currSem->blocked->hasCurr(); currSem->blocked->next()) { // Going through all PCBs blocked on current semaphore
			PCB *currPCB = currSem->blocked->getCurrPCB();
			if(currPCB->semSlice != 0 && --(currPCB->semSlice) == 0) { 	// If current PCB doesn't have unlimited blocking time and current semSlice is 1
				currPCB->valueToReturnInSemWait = 0;					// we set value for current PCB that will be returned in wait(),
				currPCB->setState(PCB::READY);							// set its state to PCB::READY,
				Scheduler::put(currPCB);								// put it in Scheduler
				currSem->blocked->remove(currPCB);						// and unblock that PCB from current semaphore
				currSem->value++; // statement says that if value of semaphore is negative, then there are abs(value) blocked threads
			}
		}
	}

	//------------------------------------//

	if(PCB::running->timeSlice != 0) { // if PCB::running doesn't have unlimited time for execution
		if(PCB::running->state == PCB::IDLE || --PCB::counter == 0) {	// if PCB::running is idle thread or if time slice expired for running thread
			dispatch();													// we switch to another thread
		}
	}
}
