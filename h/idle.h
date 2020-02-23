
#ifndef IDLE_H
#define IDLE_H

#include "thread.h"	// public Thread

class IdleThread : public Thread {

protected:
	virtual void run();
public:

	IdleThread() : Thread(defaultStackSize, 1) { // as small as possible timeSlice

	}

	virtual ~IdleThread() {
		// no waitToComplete() this time!!!
	}
};

#endif
