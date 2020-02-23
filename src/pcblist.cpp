
#include "pcblist.h"	// pcblist.cpp

#include "schedule.h"	// Scheduler::put
#include "pcb.h"		// PCB

// Deletes whole list
void ListPCB::erase() {
	while(first) {
		ListNode *old = first;
		first = first->next;
		delete old;
	}
	last = iterator = 0;
	length = 0;
}

ListPCB& ListPCB::append(PCB *pcb) {
	last = (first ? last->next : first) = new ListNode(pcb);
	length++;
	return *this;
}

ListPCB& ListPCB::remove(PCB *pcb) {
	ListNode *curr = first, *prev = 0;
	while(curr && curr->pcb != pcb) {
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

ListPCB& ListPCB::removeCurr() {
	if(iterator)
		return remove(iterator->pcb);
	else
		return *this;
}

PCB* ListPCB::popFirst() {
	PCB *ret = 0;
	if(length != 0) {
		ListNode *old = first;
		ret = old->pcb;
		if (first == last)
			first = last = 0;
		else
			first = first->next;

		length--;
		delete old;
	}
	return ret;
}

// Every PCB in this kind of list (threadsWaitingForThisToFinish) should be in state WAITING, so we should set every state to READY and move all PCBs to scheduler
void ListPCB::moveEverythingToScheduler() {
	while(first) {
		ListNode *old = first;
		first = first->next;
		old->pcb->setState(PCB::READY);
		Scheduler::put(old->pcb);
		delete old;
	}
	last = iterator = 0;
	length = 0;
}
