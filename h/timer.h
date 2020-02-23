
#ifndef TIMER_H
#define TIMER_H

#include "defs.h"	// pInterrupt, lock, unlock

class Timer {

	static pInterrupt oldInterrupt;

public:

	Timer();
	~Timer();

	static void interrupt timer(...);

};

#endif
