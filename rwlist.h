#pragma once
#include <pthread.h>

//#define DEBUG_MODE

/// TODO: complete this implementation of a thread-safe (concurrent) sorted
/// linked list of integers, which should use readers/writer locking.
class rwlist
{
	/// a node consists of a value and a pointer to another node
	struct Node
	{
		int value;
		Node* next;
	};

	/// The head of the list is referenced by this pointer
	Node* head;
	
	// Reference to the read-write lock
	mutable pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;
		
public:
	rwlist(int)
	: head(NULL)
	{
		if(pthread_rwlock_init(&lock, NULL)) {
			perror("error creating lock");
			exit(-1);
		}
	}


	/// insert *key* into the linked list if it doesn't already exist; return
	/// true if the key was added successfully.
	bool insert(int key)
	{
	#ifdef DEBUG_MODE
		perror("entering insert");
	#endif
		Node * current = head;
		Node * previous = head;
		int found = 1;
		
		pthread_rwlock_wrlock(&lock);
		// We have to confirm that the value is actually not there
		while(current != NULL) {
			if(current->value == key) {
				found = 0;
				break;
			}
			if(current->value > key) {
				found = 2;
				break;
			}
			
			previous = current;
			current = current->next;
		}
		
		if(found == 2) {
			// We can insert now as we are 1 element past where we need to be
			Node * newNode = new Node;
			newNode->value = key;
			newNode->next = current;
			previous->next = newNode;
		} else if(found == 1) {
			// The value wasn't present in the list and we never found a higher value
			if(head == current) {
				// We are the head now
				head = new Node;
				head->value = key;
				head->next = NULL;
			} else {
				// We are the tail
				Node * newNode = new Node;
				newNode->value = key;
				newNode->next = NULL;
				previous->next = newNode;
			}
		}
		
		pthread_rwlock_unlock(&lock);
	#ifdef DEBUG_MODE
		perror("exiting insert");
	#endif
		return found ? true : false;
	}
	/// remove *key* from the list if it was present; return true if the key
	/// was removed successfully.
	bool remove(int key)
	{
	#ifdef DEBUG_MODE
		perror("entering remove");
	#endif
		Node * current = head;
		Node * previous = head;
		if(current == NULL) {
			perror("shortcircuit remove");
			return false;
		}
		
		bool found = false;	
		pthread_rwlock_wrlock(&lock);
		while(current != NULL) {
			if(current->value == key) {
				previous->next = current->next;
				delete current;
				found = true;
				break;
			}
			if(current->value > key) {
				break;
			}
			
			previous = current;
			current = current->next;
		}
		pthread_rwlock_unlock(&lock);
	#ifdef DEBUG_MODE
		perror("exiting remove");
	#endif
		return found;
	}
	/// return true if *key* is present in the list, false otherwise
	bool lookup(int key) const
	{
	#ifdef DEBUG_MODE
		perror("entering lookup");
	#endif
		Node * current = head;
		if(current == NULL) {
			return false;
		}
		
		bool found = false;
		
		pthread_rwlock_rdlock(&lock);
		while(current != NULL) {
			if(current->value == key) {
				found = true;
				break;
			}
			if(current->value > key) {
				break;
			}
			
			current = current->next;
		}
		pthread_rwlock_unlock(&lock);
	#ifdef DEBUG_MODE
		perror("exiting lookup");
	#endif
		return found;
	}

	//The following are not tested by the given tester but are required for grading
	//No locks are required for these.
	size_t getSize() const
	{
		Node * current = head;
		
		if(current == NULL) {
			return -1;
		}
		
		int size = 0;
		do {
			size++;
			current = current->next;
		} while(current != NULL);
		
		return size;
	}
	int getElement(size_t idx) const
	{
		Node * current = head;
		if(current == NULL) {
			return -1;
		}
		
		int ret = -1, i = 0;
		do {
			if(i++ == idx) {
				ret = current->value;
				break;
			}
			current = current->next;
		} while(current != NULL);
		
		return ret;
	}

	//These functions just need to exist, they do not need to do anything
	size_t getBucketSize(size_t bucket) const
	{	
		return 0;
	}
	int getElement(size_t bucket, size_t idx) const
	{
		return 0;
	}
};
