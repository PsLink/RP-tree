#ifndef __KD_NODE_H
#define __KD_NODE_H

#include <vector>
using namespace std;

// -----------------------------------------------------------------------------
//	KD_Node: node of kd-tree (abstract class).
//		There are two types of kd-nodes: KD_Leaf and KD_Split.
// -----------------------------------------------------------------------------
class KD_Node {
public:
	virtual ~KD_Node() {}			// virtual destructor

	virtual void search(			// tree search
		float *query,					// query point
		float box_dist,					// box distance to query
		float ratio,					// approximation ratio
		MinK_List *list) = 0;			// top-k nn list

	virtual void traversal(			// traversal kd-tree
		vector<int> &block_size) = 0;	// leaf size (return)

	friend class KD_Tree;			// allow kd-tree to access

protected:
	float **pts_;					// data points
	int dim_;						// dimension of data points
};


// -----------------------------------------------------------------------------
//	KD_Leaf: leaf node of kd-tree.
// -----------------------------------------------------------------------------
class KD_Leaf : public KD_Node {
public:
	KD_Leaf(						// constructor
		float **pts,					// data points
		int dim,						// dimension of data points
		int n,							// num of points
		int *bkt);						// bucket

	virtual ~KD_Leaf();				// destructor

	virtual void search(			// tree search
		float *query,					// query point
		float box_dist,					// box distance to query
		float ratio,					// approximation ratio
		MinK_List *list);				// top-k nn list

	virtual void traversal(			// traversal kd-tree
		vector<int> &block_size);		// leaf size (return)

protected:
	int n_pts_;						// number of points in the bucket
	int *bkt_;						// bucket of points
};

// -----------------------------------------------------------------------------
//	KD_Split: split node of kd-tree.
// -----------------------------------------------------------------------------
class KD_Split : public KD_Node {
public:
	KD_Split(						// constructor
		float **pts,					// data points
		int dim,						// dimension of data pointss
		int cd,							// cutting dimension
		float cv,						// cutting value
		float lv,						// low bound in dim
		float hv,						// high bound in dim
		KD_Node *lc,					// left child
		KD_Node *rc);					// right child

	virtual ~KD_Split();			// destructor

	virtual void search(			// tree search
		float *query,					// query point
		float box_dist,					// box distance to query
		float ratio,					// approximation ratio
		MinK_List *list);				// top-k nn list

	virtual void traversal(			// traversal kd-tree
		vector<int> &block_size);		// leaf size (return)

protected:
	int cut_dim_;					// cutting dimension
	float cut_val_;					// cutting value
	float cd_bnds_[2];				// cutting bounds
	KD_Node* child_[2];				// children of node
};

#endif
