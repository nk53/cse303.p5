#pragma once
#include <mutex>

// prevents race conditions via mutual exclusion
std::mutex list_mutex;

/// TODO: complete this implementation of a thread-safe (concurrent) sorted
/// linked list of integers
class clist
{
	/// a Node consists of a value and a pointer to another node
	struct Node
	{
		int value;
		Node* next;
	};

	/// The head of the list is referenced by this pointer
	Node* head;

public:
    clist()
    : head(NULL)
    {}

	clist(int value)
	: head(NULL)
	{
        insert(value);
    }

	/// insert *key* into the linked list if it doesn't already exist; return
	/// true if the key was added successfully.
	bool insert(int key)
	{
        // lock is automatically released when it goes out of scope, so no
        // explicit release is required
        std::lock_guard<std::mutex> lock(list_mutex);

		if (head == NULL)
        {
            head = new Node;
            head->value = key;
            head->next = NULL;
        }
        else
        {
            // figure out where this key should go
            Node* prev = NULL;
            Node* cursor = head;
            Node* next = cursor->next;
            while ((key > cursor->value) && (next != NULL))
            {
                prev = cursor;
                cursor = next;
                next = next->next;
            }

            // only insert if it isn't already there
            if (key == cursor->value)
            {
                return false;
            }

            // do insertion
            Node *newNode = new Node;
            newNode->value = key;
            if (key > cursor->value)
            {
                // insert at end
                cursor->next = newNode;
                newNode->next = NULL;
            }
            else
            {
                // push cursor forward and insert where it was
                newNode->next = cursor;

                // make previous node point here
                if (prev != NULL)
                {
                    prev->next = newNode;
                }

                // is this also the new first node?
                if (head == cursor)
                {
                    head = newNode;
                }
            }
        }
        return true;
	}
	/// remove *key* from the list if it was present; return true if the key
	/// was removed successfully.
	bool remove(int key)
	{
        // lock is automatically released when it goes out of scope, so no
        // explicit release is required
        std::lock_guard<std::mutex> lock(list_mutex);

        // find the Node with value == key
        Node* prev = NULL;
        Node* cursor = head;
        while ((cursor != NULL) && (key > cursor->value))
        {
            prev = cursor;
            cursor = cursor->next;
        }

        if ((cursor != NULL) && (key == cursor->value))
        {
            // remove it
            if (cursor == head)
            {
                head = cursor->next;
            }
            else
            {
                prev->next = cursor->next;
            }
            delete cursor;
            return true;
        }
        // key doesn't exist in list
		return false;
	}
	/// return true if *key* is present in the list, false otherwise
	bool lookup(int key) const
	{
        // lock is automatically released when it goes out of scope, so no
        // explicit release is required
        std::lock_guard<std::mutex> lock(list_mutex);

        Node* prev = NULL;
        Node* cursor = head;
        while ((cursor != NULL) && (key > cursor->value))
        {
            prev = cursor;
            cursor = cursor->next;
        }

        if ((cursor != NULL) && (key == cursor->value))
        {
            return true;
        }
		return false;
	}


	//The following are not tested by the given tester but are required for grading
	//No locks are required for these.
	size_t getSize() const
	{
		size_t size = 0;
        Node* cursor = head;
        while (cursor != NULL)
        {
            cursor = cursor->next;
            ++size;
        }
        return size;
	}
	int getElement(size_t idx) const
	{
        Node* cursor = head;
		for (size_t i = 0; i < idx; ++i)
        {
            cursor = cursor->next;
        }
        return cursor->value;
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
