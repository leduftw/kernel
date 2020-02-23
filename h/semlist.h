
#ifndef SEMLIST_H
#define SEMLIST_H

class KernelSem;

class ListSemaphores {

	struct ListNode { // name clash? ListPCB::ListNode, ListSemaphores::listNode => No.
		KernelSem *kSem;
		ListNode *next;

		ListNode(KernelSem *kSem, ListNode *next = 0) {
			this->kSem = kSem;
			this->next = next;
		}
	};

	ListNode *first, *last;
	ListNode *iterator;
	int length;

	void erase();

protected:

	friend class PCB;
	friend class KernelSem;
	friend class Timer;

public:

	ListSemaphores() {
		first = last = iterator = 0;
		length = 0;
	}

	~ListSemaphores() {
		erase();
	}

	KernelSem* getCurrKSem() const {
		return iterator ? iterator->kSem : 0;
	}

	void toFirst() {
		iterator = first;
	}

	void next() {
		if(iterator)
			iterator = iterator->next;
	}

	int hasCurr() const {
		return iterator ? 1 : 0;
	}

	int getLength() const { // for testing
		return length;
	}

	ListSemaphores& append(KernelSem *);
	ListSemaphores& remove(KernelSem *);

};

#endif
