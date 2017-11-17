#pragma once

/// TODO: complete this implementation of a thread-safe (concurrent) hash
///       table of integers, implemented as an array of linked lists.  In
///       this implementation, each list should have a "sentinel" node that
///       contains the lock, so we can't just reuse the clist implementation.
///       In addition, the API now allows for multiple keys on each
///       operation.

class shash2
{
private:
    struct Node {
        int val;
        struct Node* next;
    };

    struct Sentinel {
        std::mutex mtx;
        struct Node* head;
    };

    // constant pointer to buckets, whose head values might change
    const struct Sentinel* buckets;
    const unsigned int numBuckets;
public:
	shash2(unsigned int _buckets)
	{
        numBuckets = _buckets;
        // allocate an array of Sentinels
        buckets = new Sentinel[numBuckets];
        // each bucket initially points to nothing
        for (int i=0; i < numBuckets; ++i)
        {
            buckets[i].head = NULL;
        }
    }

	/// insert /num/ values from /keys/ array into the hash, and return the
	/// success/failure of each insert in /results/ array.
	void insert(int* keys, bool* results, int num)
	{}
	/// remove *key* from the list if it was present; return true if the key
	/// was removed successfully.
	void remove(int* keys, bool* results, int num)
	{}
	/// return true if *key* is present in the list, false otherwise
	void lookup(int* keys, bool* results, int num) const
	{}

    // only returns false if the value already exists
    bool _insert(int key)
    {
        struct Node* current = buckets[key % numBuckets].head;
        struct Node* previous = current;

        struct Node* node = new Node;
        node->val = key;
        node->next = NULL;

        if (current == NULL)
        {
            buckets[index].head = node;

            return true;
        }

        while (current != NULL && key > current->val)
        {
            previous = current;
            current = current->next;
        }

        if (current == NULL)
        {
            // we're at the end of the list
            previous->next = node;
            return true;
        }

        // does the value already exist?
        if (current->value == key)
        {
            return false;
        }

        // we're somewhere in the middle
        previous->next = node;
        node->next = current;

        return true;
    }

    bool _remove(int key)
    {
        struct Node* current = buckets[key % numBuckets].head;
        struct Node* previous = current;

        while (current != NULL && key > current->val)
        {
            previous = current;
            current = current->next;
        }

        if (current == NULL)
        {
            // key is not in list
            return false;
        }

        previous->next = current->next;
        
        delete current;
        return true;
    }

    bool _lookup(int key) const
    {
        struct Node* current = buckets[index].head;
        while (current != NULL && key > current->val)
        {
            current = current->next;
        }

        if (key == current->val)
        {
            return true;
        }

        return false;
    }


	//The following are not tested by the given tester but are required for grading
	//No locks are required for these.

	//This refers to the number of buckets not the total number of elements.
	size_t getSize() const
	{
		return 0;
	}

	//This refers to the number of elements in a bucket, not the sentinel node.
	size_t getBucketSize() const
	{
		return 0;
	}
	int getElement(size_t bucket, size_t idx) const
	{
		return 0;
	}


	//These functions just need to exist, they do not need to do anything
	int getElement(size_t idx) const
	{
		return 0;
	}
};
