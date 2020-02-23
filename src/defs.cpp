
#include <dos.h>
#include <stdio.h>
#include <stdarg.h>

#include "defs.h"		// defs.cpp

#include "intLock.h"	// intLock, intUnlock

// Returns smaller of two given integers
int min(int l, int r) {
	return (l < r ? l : r);
}

// Thread-safe C-like printf
int syncPrintf(const char *format, ...) {
	int res;
	va_list args;

	intLock;
	va_start(args, format);
	res = vprintf(format, args);
	va_end(args);
	intUnlock;

	return res;
}
