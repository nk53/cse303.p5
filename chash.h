#include "clist.h"

#pragma once
#include<vector>

/// TODO: complete this implementation of a thread-safe (concurrent) hash
///       table of integers, implemented as an array of linked lists.
class chash
{
	/// The bucket list
	std::vector<clist> buckets;

    /// The number of buckets
    size_t size;

public:
	chash(unsigned int _buckets)
    : size(_buckets)
	{
        buckets = std::vector<clist>(size);
    }

	/// insert *key* into the appropriate linked list if it doesn't already
	/// exist; return true if the key was added successfully.
	bool insert(int key)
	{
        return buckets[key % size].insert(key);
	}
	/// remove *key* from the appropriate list if it was present; return true
	/// if the key was removed successfully.
	bool remove(int key)
	{
        return buckets[key % size].remove(key);
	}
	/// return true if *key* is present in the appropriate list, false
	/// otherwise
	bool lookup(int key) const
	{
        return buckets[key % size].lookup(key);
	}

	//The following are not tested by the given tester but are required for grading
	//No locks are required for these.

	//This refers to the number of buckets not the total number of elements.
	size_t getSize() const
	{
        return size;
	}
	size_t getBucketSize(size_t bucket) const
	{
		return buckets[bucket].getSize();
	}
	int getElement(size_t bucket, size_t idx) const
	{
		return buckets[bucket].getElement(idx);
	}



	//These functions just need to exist, they do not need to do anything
	int getElement(size_t idx) const
	{
        /*j
		for (int i = 0; i < size; ++i)
        {
            for (int j = 0; j < getBucketSize(i); ++j, --idx)
            {
                if (idx == 0)
                {
                    return buckets[i].getElement(j);
                }
            }
        }
        */
	}
};
