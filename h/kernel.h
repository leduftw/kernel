
#ifndef KERNEL_H
#define KERNEL_H

#include "pcblist.h"	// ListPCB *allPCBs
#include "semlist.h"	// ListSemaphores *allSems
#include "timer.h"		// Timer *t
#include "thread.h" 	// main thread
#include "idle.h"		// idle thread

// <dos.h> contains getvect, setvect, FP_OFF, FP_SEG

class Kernel {

	static Timer *t;
	static Thread *main;
	static IdleThread *idle;

	static void createMainThread();
	static void createIdleThread();

public:

	static ListPCB *allPCBs;
	static ListSemaphores *allSems;

	static void initialize();
	static void restore();
	static void interrupt dispatch(...);

};

#endif
