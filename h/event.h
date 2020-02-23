
#ifndef _event_h_
#define _event_h_

#include "IVTEntry.h"	// for PREPAREENTRY

typedef unsigned char IVTNo; // there are 256 entries
class KernelEv;

class Event {
public:

	Event(IVTNo ivtNo);
	~Event();

	void wait();

protected:

	friend class KernelEv;
	void signal(); // can call KernelEv

private:

	KernelEv *myImpl;

};

// Creates new interrupt routine and new IVTEntry object for entry numEntry (new interrupt routine calls old one if callOld != 0)
#define PREPAREENTRY(numEntry, callOld)\
	void interrupt inter##numEntry(...);\
	IVTEntry newEntry##numEntry(numEntry, inter##numEntry);\
	void interrupt inter##numEntry(...) {\
		newEntry##numEntry.signal();\
		if(callOld)\
			newEntry##numEntry.callOldRoutine();\
	}

#endif
