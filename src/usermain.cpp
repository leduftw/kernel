
#include "usermain.h"	// usermain.cpp

#include "defs.h"		// lock, unlock

int userMain(int argc, char *argv[]);

UserMain::UserMain(int carg, char **varg) : Thread(defaultStackSize, 2) { // change here stackSize and timeSlice for userMain
	argc = carg;
	argv = varg;
	ret = 0;
}

// We cannot put an override specifier when defining the function outside the class's member specification
void UserMain::run() {
	ret = userMain(argc, argv);
}
