
#ifndef PCB_H
#define PCB_H

#include "thread.h"		// myThread, dispatch()
#include "pcblist.h"	// threadsWaitingForThisToFinish

class PCB {

public:

	// CREATED  - thread is in this state upon creation when start() has not been called yet
	// READY    - thread is in this state when start() is called (all threads in scheduler must be in state READY)
	// WAITING  - thread is in this state when waitToComplete() is called (all threads in list threadsWaitingForThisToFinish are in this state)
	// BLOCKED  - thread is in this state when it is blocked on semaphore
	// FINISHED - thread is in this state when its run method is done
	// IDLE     - only idle thread can have this state
	enum State {CREATED, READY, WAITING, BLOCKED, FINISHED, IDLE};

private:

	static ID LAST_ID;

	ID id;
	Thread *myThread;
	StackSize stackSize;
	Time timeSlice; // if thread is created with timeSlice == 0 then it doesn't have time limit
	State state;

	int started; // for idle thread

	ListPCB *threadsWaitingForThisToFinish;

	// Context
	unsigned *stack, ss, sp, bp;

	//---------- FOR SEMAPHORES ----------//
	volatile Time semSlice;
	volatile int valueToReturnInSemWait;
	//------------------------------------//

	void createContext(); // utility method

protected:

	friend class Thread;
	friend class Kernel;
	friend class Timer;
	friend class KernelSem;
	friend class KernelEv;

public:

	static volatile int counter;	// for time sharing
	static PCB *main;				// main thread
	static PCB *idle;				// idle thread
	static PCB *running;			// current running thread

	static void wrapper();
	static PCB* getPCBById(ID id);

	static ID getRunningID() {
		return (PCB::running ? PCB::running->id : -1);
	}

	PCB(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice, Thread *myThread = 0);
	~PCB();

	void start();
	void waitToComplete();

	ID getId() const {
		return id;
	}

	State getState() const {
		return state;
	}

	void setState(State st) {
		state = st;
	}

};

#endif
