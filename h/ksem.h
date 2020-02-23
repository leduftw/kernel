
#ifndef KSEM_H
#define KSEM_H

#include "semaphor.h"	// mySemaphore
#include "pcblist.h"	// blocked

typedef unsigned int Time;

class KernelSem {

	int value;
	Semaphore *mySemaphore;
	ListPCB *blocked;

protected:

	friend class Timer;

	void block();
	void unblock(int howMany = 1);

public:

	KernelSem(int init = 1, Semaphore *mySemaphore = 0);
	virtual ~KernelSem();

	virtual int wait(Time maxTimeToWait);
	virtual int signal(int n = 0);

	int val() const {
		return value;
	}

};

#endif
