
#include "semlist.h"	// semlist.cpp

// Deletes whole list
void ListSemaphores::erase() {
	while(first) {
		ListNode *old = first;
		first = first->next;
		delete old;
	}
	last = iterator = 0;
	length = 0;
}

// Appends KernelSem to the end of list
ListSemaphores& ListSemaphores::append(KernelSem *kSem) {
	last = (first ? last->next : first) = new ListNode(kSem);
	length++;
	return *this;
}

// Removes given KernelSem from list
ListSemaphores& ListSemaphores::remove(KernelSem *kSem) {
	ListNode *curr = first, *prev = 0;
	while(curr && curr->kSem != kSem) {
		prev = curr;
		curr = curr->next;
	}

	if(!curr)
		return *this;

	if(iterator == curr)
		iterator = curr->next;

	/*
	if(iterator == curr) {
		if(curr == last)
			iterator = prev;
		else
			iterator == curr->next;
	}
	*/

	if(prev)
		prev->next = curr->next;
	else
		first = curr->next;

	if(curr == last)
		last = prev;
	delete curr;
	length--;

	return *this;
}

