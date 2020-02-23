
#include <iostream.h>	// cout, endl

#include "kernel.h"		// initialize, restore
#include "usermain.h"	// UserMain
#include "intlock.h"	// intLock, intUnlock
#include "defs.h"		// syncPrintf

int main(int argc, char *argv[]) {
	Kernel::initialize();

	intLock;
	UserMain *uMain = new UserMain(argc, argv);
	intUnlock;

	uMain->start();

	uMain->waitToComplete(); // not just delete uMain because we don't know what is main's return value
	int ret = uMain->getRet();

	intLock;
	delete uMain;
	intUnlock;

	syncPrintf("return value: %d\n", ret);

	Kernel::restore();

	return ret;
}
