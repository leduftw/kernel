
#include "semaphor.h"	// semaphor.cpp

#include "defs.h"		// lock, unlock
#include "ksem.h"		// myImpl

Semaphore::Semaphore(int init) {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	myImpl = new KernelSem(init, this);
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

Semaphore::~Semaphore() {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	delete myImpl;
	myImpl = 0;
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

int Semaphore::wait(Time maxTimeToWait) {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	int ret = 1; // default value
	if(myImpl)
		ret = myImpl->wait(maxTimeToWait);
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
	return ret;
}

int Semaphore::signal(int n) {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	int ret = -1;
	if(myImpl)
		ret = myImpl->signal(n);
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
	return ret;
}

int Semaphore::val() const {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	int ret = -9999;
	if(myImpl)
		ret = myImpl->val();
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
	return ret;
}
