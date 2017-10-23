#include "headers.h"

// -----------------------------------------------------------------------------
//  MinK_List:
//  An MinK_List structure is one which maintains the smallest k values (of 
//  type float) and associated object id (of type int).
//
//  It is currently implemented using an array with k items. Items are 
//  stored in descending order by key, and its insertion is made through
//  standard insertion mode. (It is quite inefficient if k is large, but 
//  current applications are able to handle since the value of k is small.)
//
//  Note that the priority queue contains k + 1 entries, while the last
//  entry is used as a simple place holder and is otherwise ignored.
// -----------------------------------------------------------------------------
MinK_List::MinK_List(				// constructor (given max size)
	int max)							// max size
{
	num_ = 0;
	k_ = max;
	mk_ = new MinK_Node[max + 1];
}

// -----------------------------------------------------------------------------
MinK_List::~MinK_List() 			// destructor
{
	if (mk_ != NULL) {
		delete[] mk_; mk_ = NULL;
	}
}

// -----------------------------------------------------------------------------
void MinK_List::reset()				// make exist queue empty
{
	num_ = 0;						// setting current active num = 0
}

// -----------------------------------------------------------------------------
float MinK_List::min_key()			// return minimum key
{
	return (num_ > 0 ? mk_[0].key : MAXREAL);
}

// -----------------------------------------------------------------------------
float MinK_List::max_key()			// return maximum key
{
	return (num_ == k_ ? mk_[k_ - 1].key : MAXREAL);
}

// -----------------------------------------------------------------------------
float MinK_List::ith_smallest_key(	// return ith key
	int i)								// ith position
{
	return (i < num_ ? mk_[i].key : MAXREAL);
}

// -----------------------------------------------------------------------------
int MinK_List::ith_smallest_id(		// return ith info
	int i)								// ith position
{
	return (i < num_ ? mk_[i].id : MININT);
}

// -----------------------------------------------------------------------------
bool MinK_List::isFull()			// is full?
{
	if (num_ >= k_) return true;
	else return false;
}

// -----------------------------------------------------------------------------
int MinK_List::size()				// size of list
{
	return num_;
}

// -----------------------------------------------------------------------------
void MinK_List::insert(				// insert item (inline for speed)
	float key,							// key of item
	int id)								// id of item
{
	int i = -1;
	int pos = -1;
	bool already_in = false;

	for (i = 0; i < num_; i++) {
		if (mk_[i].id == id) {
			already_in = true;
			break;
		}
		else if (key < mk_[i].key) break;
	}

	pos = i;
	if (!already_in) {
		for (i = num_; i > pos; i--) {
			mk_[i] = mk_[i - 1];
		}
		mk_[pos].key = key;
		mk_[pos].id = id;

		if (num_ < k_) num_++;
	}

	/*
	int i = -1;
	for (i = num_; i > 0; i--) {
	if (mk_[i-1].key > key && mk_[i - 1].id != id) mk_[i] = mk_[i-1];
	else break;
	}
	mk_[i].key = key;				// store new item here
	mk_[i].id = id;
	if (num_ < k_) num_++;			// increament number of items
	*/
}


// -----------------------------------------------------------------------------
//  Pri_Queue:
//  An Pri_Queue structure is a list of items, where the items are indexed
//	in an ascending order by key. The basic operations are insert() and
//	extr_min().
//
//	The priority queue is maintained using a standard binary heap.
//	(Implementation Note: indexing is performed from [1..max] rather than
//	the standard of [0..max-1], which simplifies parent/child computation.)
// -----------------------------------------------------------------------------
Pri_Queue::Pri_Queue(				// constructor (given max size)
	int max)							// max size
{
	num_ = 0;
	max_size_ = max;
	pq_ = new PQ_Node[max + 1];
}

// -----------------------------------------------------------------------------
Pri_Queue::~Pri_Queue()				// destructor
{
	if (pq_ != NULL) {
		delete[] pq_; pq_ = NULL;
	}
}

// -----------------------------------------------------------------------------
bool Pri_Queue::empty()				// is empty ?
{
	if (num_ == 0) return true;
	else return false;
}

// -----------------------------------------------------------------------------
bool Pri_Queue::non_empty()			// is not empty ?
{
	if (num_ == 0) return false;
	else return true;
}

// -----------------------------------------------------------------------------
void Pri_Queue::reset()				// make exist queue empty
{
	num_ = 0;						// setting current active num = 0
}

// -----------------------------------------------------------------------------
void Pri_Queue::insert(				// insert item (inline for speed)
	float key,							// key of item
	int id)								// id of item
{
	if (++num_ > max_size_) {
		error("Priority Queue Overflow!", false);
	}

	int r = num_;					// new position
	while (r > 1) {					// find proper place
		int p = r / 2;				// compare with parent
		if (pq_[p].key <= key) break;// in proper place

		pq_[r] = pq_[p];			// swap with parent
		r = p;
	}
	pq_[r].key = key;				// insert new item in proper place
	pq_[r].id = id;
}

// -----------------------------------------------------------------------------
void Pri_Queue::extr_min(			// extract min item (then delete it)
	float& key,							// key of min item (returned)
	int& id)							// id of min item (returned)
{
	key = pq_[1].key;				// extract min item
	id = pq_[1].id;
	// delete min item from pri queue
	float kn = pq_[num_--].key;		// last item in queue
	int p = 1;						// p point to item out of position
	int r = p << 1;					// left child of p

	while (r <= num_) {				// set r to larger child of p
		if (r < num_ && pq_[r].key > pq_[r + 1].key) r++;
		if (kn <= pq_[r].key) break;// in proper order

		pq_[p] = pq_[r];			// swap with child
		p = r;
		r = p << 1;
	}
	pq_[p] = pq_[num_ + 1];			// insert last item
}
