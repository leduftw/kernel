
#include "kev.h"		// kev.cpp

#include "defs.h"		// lock, unlock
#include "thread.h"		// dispatch()
#include "schedule.h"	// Scheduler::put
#include "ivtentry.h"	// IVTEntry::IVT

KernelEv::KernelEv(IVTNo ivtNo) {
	numEntry = ivtNo;
	owner = PCB::running;
	value = 0;
	blocked = 0;

	if(IVTEntry::IVT[numEntry]) {
		// COMMENT OUT IF SOMETHING BREAKS!
		if (IVTEntry::IVT[ivtNo]->myEvent) {
			IVTEntry::IVT[ivtNo]->myEvent->signal();
		}
		IVTEntry::IVT[numEntry]->myEvent = this;
	}
}

KernelEv::~KernelEv() {
	if(IVTEntry::IVT[numEntry] && IVTEntry::IVT[numEntry]->myEvent == this)
		IVTEntry::IVT[numEntry]->myEvent = 0;

	if(owner->state == PCB::BLOCKED)
		signal();
}

void KernelEv::wait() {
	if(PCB::running == owner) {	// if some thread that didn't create this event calls wait, it should do nothing
		if(value == 1)
			value = 0;
		else {	// value == 0
			blocked = 1;
			PCB::running->state = PCB::BLOCKED; // same as owner->state = PCB::BLOCKED;
			dispatch();
		}
	}
}

void KernelEv::signal() {
	if (blocked == 1) {
		blocked = 0;
		owner->state = PCB::READY;
		// Proveriti ispravnost ovoga jos jednom. U tekstu receno put i get, a ja ovde radim put pa dispatch koji radi put i get.
		// Ipak bi trebalo da je ispravno jer owner != PCB::running. Owner je sigurno blokiran ako smo dosli ovde, dispatch stavlja PCB::running, a mi stavljamo owner-a.
		Scheduler::put(owner);
		dispatch(); // said in the statement
	} else
		value = 1;
	/*
	// THE FOLLOWING SOLUTION IS NOT GOOD:
	// Owner can create semaphore and event. If owner is blocked on semaphore and some other thread is running and interrupt happens
	// e.g. keyboard interrupt, then this solution will unblock owner even though owner may not even called e.wait()
	if(owner->state == PCB::BLOCKED) {
		owner->state = PCB::READY;
		Scheduler::put(owner);
		dispatch(); // said in the statement
	} else
		value = 1;
	*/
}
