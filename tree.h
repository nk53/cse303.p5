#pragma once
#include <pthread.h>

#define BREAKUP_CONSTANT 16

/// TODO: complete this implementation of a thread-safe (concurrent) AVL
///       tree of integers, implemented as a set of Node objects.
///       In addition, the API now allows for multiple insertions on each
///       operation.
///       Less than goes to the left. Greater than goes to the right.
class tree
{
//This is for testing purposes. Do not access your Node directly.
public:
	struct Node
	{
	public:
		short value;
		short height;
		Node* left;
		Node* right;
	};
	Node * head;
private:
	unsigned elements = 0;
	
	// The RW-Lock
	mutable pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;
	
	// Returns the idx'th element from the tree assuming an inorder traversal.
	int getElement(size_t idx, Node* at) const {
		return 0;
	}
	
	int getHeight(Node * node) {
		return node ? node->height : 0;
	}
	int max(int x, int y) {
		return (x > y) ? x : y;
	}
	
	// Get the balance for a node
	int getBalance(Node * node) {
		return node ? getHeight(node->left) - getHeight(node->right) : 0;
	}
	
	// Finds the smallest value in the tree
	Node * findMinValue(Node * node) {
		Node * current = node;
		Node * previous = node;
		while(current) {
			previous = current;
			current  = current->left;
		}
		
		return previous;
	}
	
	/* Rotations for tree */
	Node * rotateRight(Node * y) {
		Node * x = y->left;
		Node * T2 = x->right;
		
		// Perform rotation
		x->right = y;
		y->left = T2;
		
		// Update heights
		y->height = max(getHeight(y->left), getHeight(y->right));
		x->height = max(getHeight(x->left), getHeight(x->right));
		
		// Heights must be 1 larger than child
		y->height++;
		x->height++;
		
		return x;
	}
	Node * rotateLeft(Node * x) {
		Node * y = x->right;
		Node * T2 = y->left;
		
		// Perform rotation
		y->left = x;
		x->right = T2;
		
		// Update heights
		x->height = max(getHeight(x->left), getHeight(x->right));
		y->height = max(getHeight(y->left), getHeight(y->right));
		x->height++;
		y->height++;
		
		return y;
	}
	
	Node * insertValue(Node * node, int value, bool * result) {
		if(!node) {
			Node * n = new Node;
			n->left = NULL;
			n->right = NULL;
			n->height = 1;
			n->value = value;
			elements++;
			
			return n;
		}
		
		// Insert into tree
		int nodeValue = node->value;
		if(value < nodeValue) {
			// Less than to the left
			node->left = insertValue(node->left, value, result);
		} else if(value > nodeValue) {
			// Greater than to the right
			node->right = insertValue(node->right, value, result);
		} else {
			*result = false;
			// Equals to
			return node;
		}
		
		// Update the height of the node
		node->height = max(getHeight(node->left), getHeight(node->right)) + 1;
		
		// Get the balance factor
		int balance = getBalance(node);
		if(balance > 1) {
			int v = node->left->value;
			if(value < v) {
				node = rotateRight(node);
			} else if(value > v) {
				node->left = rotateLeft(node->left);
				node = rotateRight(node);
			}
		} else if(balance < -1) {
			int v = node->right->value;
			if(value > v) {
				node = rotateLeft(node);
			} else if(value < v) {
				node->right = rotateRight(node->right);
				node = rotateLeft(node);
			}
		}
		
		return node;
	}
	
	Node * removeValue(Node * node, int value, bool * result) {
		// If node is NULL
		if(!node) {
			return node;
		}
		
		if(value < node->value) {
			// Our target is less than
			node = removeValue(node->left, value, result);
		} else if(value > node->value) {
			// Our target is greater
			node = removeValue(node->right, value, result);
		} else {
			*result = true;
			elements--;
			// We are at the correct node to delete
			if(!(node->left) && !(node->right)) {
				// No children, easy mode
				free(node);
				node = NULL;
			} else {
				Node * tmp = NULL;
				if(!(node->left)) {
					// Left node DNE
					tmp = node->right;
				} else if(!(node->right)) {
					// Right node DNE
					tmp = node->left;
				}
				
				if(tmp) {
					// Copy everything from tmp to current node
					node->value  = tmp->value;
					node->left   = tmp->left;
					node->right  = tmp->right;
					node->height = tmp->height;
					
					// Clear out tmp
					free(tmp);
					tmp = NULL;
				} else {
					// We have 2 children to deal with
					int val = findMinValue(node->right)->value;
					
					// Swap values of min and temp
					node->value = val;
					
					// Remove min
					node->right = removeValue(node->right, val, result);
				}
			}
		}
		if(!node) {
			return node;
		}
		
		// Update node height
		node->height = max(getHeight(node->left), getHeight(node->right)) + 1;
		
		// Get the balance factor
		int balance = getBalance(node);
		if(balance > 1) {
			if(getBalance(node->left) >= 0) {
				node = rotateRight(node);
			} else {
				node->left = rotateLeft(node->left);
				node = rotateRight(node);
			}
		} else if(balance < -1) {
			if(getBalance(node->right) >= 0) {
				node = rotateLeft(node);
			} else {
				node->right = rotateRight(node->right);
				node = rotateLeft(node);
			}
		}
		
		return node;
	}
	
	bool lookupValue(Node * node, int value) const {
		if(!node) {
			return false;
		}
		
		if(value < node->value) {
			return lookupValue(node->left, value);
		} else if(value > node->value) {
			return lookupValue(node->right, value);
		}
		
		return true;
	}

#ifdef DEBUG_MODE	
	void printTree(Node * node) {
		if(node->left) {
			printTree(node->left);
		}
		if(node->right) {
			printTree(node->right);
		}
		
		printf("%d ", node->value);
	}
#endif
public:
	tree(int)
	: head(NULL)
	{
		if(pthread_rwlock_init(&lock, NULL)) {
			perror("error creating lock");
			exit(-1);
		}
	}
	
	/// insert /num/ values from /data/ array into the tree, and return the
	/// success/failure of each insert in /results/ array.
	void insert(int* data, bool* results, int num) {
		int i, j, k;
		for(i = 0; i < num; i++) {
			results[i] = true;
		}
		
		for(j = 0, k = num / BREAKUP_CONSTANT; j < k; j+=i) {
			pthread_rwlock_wrlock(&lock);
			for(i = 0; i < BREAKUP_CONSTANT; i++) {
				head = insertValue(head, data[j + i], &(results[j + i]));
			}
			pthread_rwlock_unlock(&lock);
		}
		
		k = num % BREAKUP_CONSTANT;
		if(k) {
			pthread_rwlock_wrlock(&lock);
			for(i = 0; i < k; i++) {
				head = insertValue(head, data[j + i], &results[j + i]);
			}
			pthread_rwlock_unlock(&lock);
		}
	}
	
	/// remove *data* from the list if it was present; return true if the data
	/// was removed successfully.
	void remove(int* data, bool* results, int num) {
		int i, j, k;
		for(i = 0; i < num; i++) {
			results[i] = false;
		}
		
		for(j = 0, k = num / BREAKUP_CONSTANT; j < k; j+=i) {
			pthread_rwlock_wrlock(&lock);
			for(i = 0; i < BREAKUP_CONSTANT; i++) {
				head = removeValue(head, data[j + i], &(results[j + i]));
			}
			pthread_rwlock_unlock(&lock);
		}
		
		k = num % BREAKUP_CONSTANT;
		if(k) {
			pthread_rwlock_wrlock(&lock);
			for(i = 0; i < k; i++) {
				head = removeValue(head, data[j + i], &(results[j + 1]));
			}
			pthread_rwlock_unlock(&lock);
		}
	}
	/// return true if *data* is present in the list, false otherwise
	void lookup(int* data, bool* results, int num) const {
		int i, j, k;
		
		for(j = 0, k = num / BREAKUP_CONSTANT; j < k; j+=i) {
			pthread_rwlock_rdlock(&lock);
			for(i = 0; i < BREAKUP_CONSTANT; i++) {
				results[j + i] = lookupValue(head, data[j + i]);
			}
			pthread_rwlock_unlock(&lock);
		}
		
		k = num % BREAKUP_CONSTANT;
		if(k) {
			pthread_rwlock_wrlock(&lock);
			for(i = 0; i < k; i++) {
				results[j + i] = lookupValue(head, data[j + i]);
			}
			pthread_rwlock_unlock(&lock);
		}
	}
	
	//The following are not tested by the given tester but are required for grading
	//No locks are required for these.

	//Total number of elements in the tree
	size_t getSize() const
	{
		return elements;
	}

	int getElement(size_t idx) const
	{
		return getElement(idx, head);
	}


	//These functions need to exist, they do not need to do anyting

	size_t getBucketSize() const
	{
		return 0;
	}
	int getElement(size_t bucket, size_t idx) const
	{
		return 0;
	}
};
