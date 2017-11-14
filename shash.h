#pragma once
#include <mutex>

/// TODO: complete this implementation of a thread-safe (concurrent) hash
///       table of integers, implemented as an array of linked lists.  In
///       this implementation, each list should have a "sentinel" node that
///       contains the lock, so we can't just reuse the clist implementation

struct Node {
	int val;
	struct Node * next;
};

struct SentinelNode {
	std::mutex mtx;
	struct Node * head;
};

struct SentinelNode * buckets;
unsigned numBuckets;

class shash
{
public:
	shash(unsigned _buckets)
	{
		numBuckets = _buckets;
		buckets = (struct SentinelNode *)malloc(sizeof(struct SentinelNode) * numBuckets);
		int i;
		for(i = 0; i < numBuckets; i++) {
			buckets[i].head = NULL;
		}
	}


	/// insert *key* into the appropriate linked list if it doesn't already
	/// exist; return true if the key was added successfully.
	bool insert(int key)
	{
		// Lock current bucket
		int index = key % numBuckets;
		bool valid = true;
		
		// Lock mutex
		buckets[index].mtx.lock();
		
		// Get trackers
		struct Node * current = buckets[index].head;
		struct Node * previous = current;
		
		// Create the node to insert
		struct Node * node = (struct Node *)malloc(sizeof(struct Node));
		node->val  = key;
		node->next = NULL;
		
		// Cases that involve modifying values in the SentinelNodes
		if(current == NULL) {
			// Create a head
			(&(buckets[index]))->head = node;
			
			// We will escape this function here
			buckets[index].mtx.unlock();
			return valid;
		} else if(current->val > key) {
			// Check to see if we should be before the head
			node->next = (buckets[index]).head;
			(&(buckets[index]))->head = node;
			
			// Escape the function here
			buckets[index].mtx.unlock();
			return valid;
		}
		
		// Itterate through the linked list
		while(current != NULL) {
			if(current->val == key) {
				valid = false;
				break;
			}
			if(current->val > key) {
				break;
			}
			
			previous = current;
			current = current->next;
		}
		
		// Only insert if not found
		if(valid) {
			if(current) {
				// Insert between previous and current
				node->next = current;
				previous->next = node;
			} else {
				// Insert at the tail
				previous->next = node;
			}
		}
		
		// Unlock mutex
		buckets[index].mtx.unlock();		
		return valid;
	}
	/// remove *key* from the appropriate list if it was present; return true
	/// if the key was removed successfully.
	bool remove(int key)
	{
		// Lock current bucket
		int index = key % numBuckets;
		bool found = false;
		
		// Lock mutex
		buckets[index].mtx.lock();
		
		// Trackers
		struct Node * current = buckets[index].head;
		struct Node * previous = current;
		
		while(current != NULL) {
			if(current->val == key) {
				// Remove element
				if(current == buckets[index].head) {
					// We are the head
					(&(buckets[index]))->head = current->next;
				} else {
					// We are inside the linked list
					previous->next = current->next;
				}
				
				// Clear out current
				current->val  = 0;
				current->next = NULL;
				free(current);
				
				// Mark as found
				found = true;
				break;
			}
			if(current->val > key) {
				break;
			}
			
			previous = current;
			current = current->next;
		}
		
		// Unlock mutex
		buckets[index].mtx.unlock();
		return found;
	}
	/// return true if *key* is present in the appropriate list, false
	/// otherwise
	bool lookup(int key) const
	{
		// Lock current bucket
		int index = key % numBuckets;
		bool found = false;
		
		// Lock mutex
		buckets[index].mtx.lock();
		
		// Itterate through linked list
		struct Node * current = buckets[index].head;
		while(current != NULL) {
			if(current->val == key) {
				found = true;
				break;
			}
			if(current->val > key) {
				break;
			}
			
			current = current->next;
		}
		
		// Unlock mutex
		buckets[index].mtx.unlock();
		return found;
	}

	//The following are not tested by the given tester but are required for grading
	//No locks are required for these.

	//This refers to the number of buckets not the total number of elements.
	size_t getSize() const
	{
		return numBuckets;
	}

	//This refers to the number of elements in a bucket, not the sentinel node.
	size_t getBucketSize(size_t bucket) const
	{
		int counter = 0;
		struct Node * current = buckets[bucket].head;
		while(current != NULL) {
			counter++;
			current = current->next;
		}
		return counter;
	}
	int getElement(size_t bucket, size_t idx) const
	{
		int ret = -1;
		
		int counter = idx;
		struct Node * current = buckets[bucket].head;
		while(current != NULL) {
			if(counter-- <= 0) {
				ret = current->val;
				break;
			}
			current = current->next;
		}
		return ret;
	}


	//These functions just need to exist, they do not need to do anything
	int getElement(size_t idx) const
	{
		return 0;
	}
};
