
#ifndef IVTENTRY_H
#define IVTENTRY_H

#include "defs.h"	// lock, unlock, pInterrupt
#include "kev.h"	// myEvent

typedef unsigned char IVTNo;

class IVTEntry {

public:

	static IVTEntry* IVT[256];

	IVTEntry(IVTNo ivtNo, pInterrupt newRoutine);
	~IVTEntry();

	void signal();
	void callOldRoutine();

	pInterrupt newRoutine;

protected:

	friend class KernelEv;

private:

	KernelEv *myEvent;
	pInterrupt oldRoutine;
	IVTNo numEntry;

};

#endif
