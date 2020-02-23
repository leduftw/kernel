
#include "event.h"	// event.cpp

#include "kev.h"	// myImpl
#include "defs.h"	// lock, unlock

Event::Event(IVTNo ivtNo) {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	myImpl = new KernelEv(ivtNo);
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

Event::~Event() {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	if(myImpl) {
		delete myImpl;
		myImpl = 0;
	}
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

void Event::wait() {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	if(myImpl)
		myImpl->wait();
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

void Event::signal() {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	if(myImpl)
		myImpl->signal();
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}
