
#ifndef PCBLIST_H
#define PCBLIST_H

class PCB; // There is no need to include header file when we only use name, but it would be necessary if we used constructors, methods, fields, etc.

class ListPCB {

	struct ListNode {
		PCB *pcb;
		ListNode *next;

		ListNode(PCB *pcb, ListNode *next = 0) {
			this->pcb = pcb;
			this->next = next;
		}
	};

	ListNode *first, *last;
	ListNode *iterator; // in this version of C++ we cannot say mutable or nullptr
	int length;

	void erase();

protected:

	friend class PCB;
	friend class Timer;

public:

	ListPCB() {
		first = last = iterator = 0;
		length = 0;
	}

	/* THIS VERSION OF C++ DOES NOT SUPPORT DELETED FUNCTIONS (only after C++11)
	// Note: Scott Meyers mentions in his Effective Modern C++ book, that
	// deleted functions should generally be public as it results in better
	// error messages due to the compilers behavior to check accessibility
	// before deleted status
	ListPCB(const ListPCB &) = delete;
	ListPCB& operator=(const ListPCB &) = delete;
	*/

	~ListPCB() {
		erase();
	}

	PCB* getCurrPCB() const {
		return iterator ? iterator->pcb : 0;
	}

	void toFirst() {
		iterator = first;
	}

	void next() {
		if(iterator)
			iterator = iterator->next;
	}

	int hasCurr() const {
		return iterator ? 1 : 0; // this version of C++ does not have bool type, and doesn't know for true and false literals
	}

	int getLength() const { // for testing
		return length;
	}

	ListPCB& append(PCB *);	// appends given PCB to the end of list
	ListPCB& remove(PCB *);	// removes given PCB from list
	ListPCB& removeCurr();	// removes PCB to which iterator points
	PCB* popFirst(); // removes first PCB in this list
	void moveEverythingToScheduler(); // Every PCB in this list is moved to scheduler in FIFO order (list will be empty after this)

};

#endif
