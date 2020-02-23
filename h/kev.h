
#ifndef KEV_H
#define KEV_H

#include "pcb.h"	// owner

typedef unsigned char IVTNo;

class KernelEv {
public:

	KernelEv(IVTNo ivtNo);
	~KernelEv();

	void wait();

protected:

	friend class Event;
	friend class IVTEntry;
	void signal();

private:

	PCB *owner;	// the only thread that can block on this is event is the one which created it
	IVTNo numEntry;
	int value, blocked;

};

#endif
