
#ifndef USERMAIN_H
#define USERMAIN_H

#include "thread.h"	// public Thread

class UserMain : public Thread {

	int argc, ret;
	char **argv;

protected:

	virtual void run(); // doesn't work with override (old C++)!

public:

	// default constructor for Thread is also called
	UserMain(int carg, char **varg);

	virtual ~UserMain() {
		waitToComplete(); // MANDATORY!!!
		// Otherwise it doesn't work good, e.g. main thread has timeSlice == 0 
		// and if userMain is started with start() but isn't really begun its run method
		// and main thread calls delete userMain, then userMain won't start its run method at all
		// Everything is consequence of order of destructor calls
	}

	int getRet() const {
		return ret;
	}

};

#endif
