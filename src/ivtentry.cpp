
#include <dos.h>		// getvect, setvect

#include "ivtentry.h"	// ivtentry.cpp

IVTEntry* IVTEntry::IVT[256] = { 0 };

IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt newRoutine) {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	IVTEntry::IVT[ivtNo] = this;
	numEntry = ivtNo;
	oldRoutine = 0; // just to calm down eclipse
	myEvent = 0;
	this->newRoutine = newRoutine;
#ifndef BCC_BLOCK_IGNORE
	oldRoutine = getvect(numEntry);
	setvect(numEntry, newRoutine);
	unlock;
#endif
}

IVTEntry::~IVTEntry() {
#ifndef BCC_BLOCK_IGNORE
	lock;
	setvect(numEntry, oldRoutine);
#endif
	oldRoutine(); // unknown reason
	signal(); // unknown reason
	IVTEntry::IVT[numEntry] = 0;
	myEvent = 0;
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

void IVTEntry::signal() {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	if(myEvent)
		myEvent->signal();
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

void IVTEntry::callOldRoutine() {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	if(oldRoutine)
		oldRoutine();
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}
