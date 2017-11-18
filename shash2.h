#pragma once
#include <queue>
#include <mutex>

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
        int value;
        struct Node* next;
    };

    struct Sentinel {
        std::mutex lock;
        struct Node* head;
    };

    // constant pointer to buckets, whose head values might change
    struct Sentinel* buckets;
    unsigned int numBuckets;
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

    ~shash2()
    {
        for (int i = 0; i < numBuckets; ++i)
        {
            struct Sentinel* bucket = &buckets[i];
            struct Node* node = bucket->head;
            struct Node* next = NULL;

            while (node != NULL)
            {
                next = node->next;
                delete node;
                node = next;
            }
        }

        delete[] buckets;
    }

	/// insert /num/ values from /keys/ array into the hash, and return the
	/// success/failure of each insert in /results/ array.
	void insert(int* keys, bool* results, int num)
	{
        // keeps track of how many updates need to be done in each bucket
        std::queue<int> * update_indices = new std::queue<int>[numBuckets];
        for (int i=0; i < num; ++i)
        {
            const int bucket = keys[i] % numBuckets;
            update_indices[bucket].push(i);
        }

        // acquire one lock for each bucket that is being updated
        for (int bucket_index=0; bucket_index < numBuckets; ++bucket_index)
        {
            if (update_indices[bucket_index].size() > 0)
            {
                struct Sentinel* bucket = &buckets[bucket_index];
                // acquire the lock
                std::lock_guard<std::mutex> lock(bucket->lock);

                // do this bucket's updates
                while (update_indices[bucket_index].size() > 0)
                {
                    // handle the queue
                    int key_index = update_indices[bucket_index].front();
                    update_indices[bucket_index].pop();

                    // do the actual insert
                    results[key_index] = _insert(keys[key_index]);
                }
            }
        }

        delete[] update_indices;
    }

	/// remove *key* from the list if it was present; return true if the key
	/// was removed successfully.
	void remove(int* keys, bool* results, int num)
	{
        // keeps track of how many updates need to be done in each bucket
        std::queue<int> * update_indices = new std::queue<int>[numBuckets];
        for (int i=0; i < num; ++i)
        {
            const int bucket = keys[i] % numBuckets;
            update_indices[bucket].push(i);
        }

        // acquire one lock for each bucket that is being updated
        for (int bucket_index=0; bucket_index < numBuckets; ++bucket_index)
        {
            if (update_indices[bucket_index].size() > 0)
            {
                struct Sentinel* bucket = &buckets[bucket_index];
                // acquire lock
                std::lock_guard<std::mutex> lock(bucket->lock);
                
                // do this bucket's updates
                while (update_indices[bucket_index].size() > 0)
                {
                    // handle the queue
                    int key_index = update_indices[bucket_index].front();
                    update_indices[bucket_index].pop();

                    // do the actual insert
                    results[key_index] = _remove(keys[key_index]);
                }
            }
        }

        delete[] update_indices;
    }

	/// return true if *key* is present in the list, false otherwise
	void lookup(int* keys, bool* results, int num) const
	{
        std::queue<int> * update_indices = new std::queue<int>[numBuckets];
        for (int i=0; i < num; ++i)
        {
            const int bucket = keys[i] % numBuckets;
            update_indices[bucket].push(i);
        }

        // acquire one lock for each bucket that is being updated
        for (int bucket_index=0; bucket_index < numBuckets; ++bucket_index)
        {
            if (update_indices[bucket_index].size() > 0)
            {
                struct Sentinel* bucket = &buckets[bucket_index];
                // acquire lock
                std::lock_guard<std::mutex> lock(bucket->lock);

                // do this bucket's updates
                while (update_indices[bucket_index].size() > 0)
                {
                    // handle the queue
                    int key_index = update_indices[bucket_index].front();
                    update_indices[bucket_index].pop();

                    // do the actual insert
                    results[key_index] = _lookup(keys[key_index]);
                }
            }
        }

        delete[] update_indices;
    }

    // only returns false if the value already exists
    bool _insert(int key)
    {
        struct Sentinel* bucket = &buckets[key % numBuckets];

        // acquire lock
        //std::lock_guard<std::mutex> lock(bucket->);
        struct Node* current = bucket->head;
        struct Node* previous = current;

        if (current == NULL)
        {
            struct Node* node = new Node;
            node->value = key;
            node->next = NULL;

            bucket->head = node;

            return true;
        }

        while (current != NULL && key > current->value)
        {
            previous = current;
            current = current->next;
        }

        if (current == NULL)
        {
            // we're at the end of the list
            struct Node* node = new Node;
            node->value = key;
            node->next = NULL;

            previous->next = node;
            return true;
        }

        // does the value already exist?
        if (current->value == key)
        {
            return false;
        }

        struct Node* node = new Node;
        node->value = key;
        node->next = NULL;

        if (current == previous)
        {
            // are we at the head?
            bucket->head = node;
        }
        else
        {
            // we're somewhere in the middle
            previous->next = node;
        }

        node->next = current;

        return true;
    }

    bool _remove(int key)
    {
        struct Sentinel* bucket = &buckets[key % numBuckets];

        // acquire lock
        //std::lock_guard<std::mutex> lock(bucket->mtx);
        struct Node* current = bucket->head;
        struct Node* previous = current;

        while (current != NULL && key > current->value)
        {
            previous = current;
            current = current->next;
        }

        if (current == NULL)
        {
            // key is not in list
            return false;
        }

        if (current == previous)
        {
            // head needs to be replaced
            bucket->head = current->next;
        }
        else
        {
            // we're somewhere in the middle
            previous->next = current->next;
        }
        
        delete current;

        return true;
    }

    bool _lookup(int key) const
    {
        struct Sentinel* bucket = &buckets[key % numBuckets];

        // acquire lock
        //std::lock_guard<std::mutex> lock(bucket->mtx);
        struct Node* current = buckets[key % numBuckets].head;

        while (current != NULL && key > current->value)
        {
            current = current->next;
        }

        if (current == NULL)
        {
            return false;
        }

        if (key == current->value)
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
		return (size_t) numBuckets;
	}

	//This refers to the number of elements in a bucket, not the sentinel node.
	size_t getBucketSize() const
	{
		return 0;
	}

    size_t getBucketSize(size_t bucket)
    {
        struct Node* current = buckets[bucket].head;

        size_t size = 0;
        while (current != NULL)
        {
            ++size;
            current = current->next;
        }

        return size;
    }

    // returns 0 or the value at the requested index
	int getElement(size_t bucket, size_t idx) const
	{
		struct Node* current = buckets[bucket].head;

        while (idx > 0 && current != NULL)
        {
            current = current->next;
            --idx;
        }

        if (idx == 0)
        {
            return current->value;
        }

        return 0;
	}


	//These functions just need to exist, they do not need to do anything
	int getElement(size_t idx) const
	{
        for (int i=0; i < numBuckets; ++i)
        {
            struct Node* current = buckets[i].head;
            while (current != NULL && idx > 0)
            {
                current = current->next;
                --idx;
            }

            if (current != NULL && idx == 0)
            {
                return current->value;
            }
        }

        return 0;
	}
};
