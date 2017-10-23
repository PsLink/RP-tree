#pragma once
#ifndef __KD_TREE_H
#define __KD_TREE_H

class KD_Node;

// -----------------------------------------------------------------------------
//	KD_Tree: structure for approximate and exact nearest neighbor search
// -----------------------------------------------------------------------------
class KD_Tree {
public:
	KD_Tree(						// constructor
		float **pts,					// point array
		int n,							// number of points
		int d,							// dimensionality
		int kd_leaf_size);				// leaf size of kd-tree

	~KD_Tree();						// destructor

	// -------------------------------------------------------------------------
	void traversal(					// traversal kd-tree to get leaf info
		vector<int> &block_size,		// leaf size (return)
		int *object_id);				// object id with leaf order (return)

protected:
	float **pts_;					// data points
	int dim_;						// dimensionality
	int n_pts_;						// number of point
	int kd_leaf_size_;				// leaf size of kd-tree

	int *pidx_;						// point indices (bkt_ in KD_Node)
	KD_Node *root_;					// root of kd-tree

	// float *bnd_box_low_;			// bounding box low point
	// float *bnd_box_high_;			// bounding box high point

	// -------------------------------------------------------------------------
	// void calc_encl_rect(			// calc smallest enclosing rectangle
	// 	KD_Rect &bnds);					// bounding box (return)

	// -------------------------------------------------------------------------
	KD_Node* rkd_tree(				// recursive build kd-tree
		int *pidx,						// point index
		int n);							// number of points
		// KD_Rect	&bnd_box);				// bounding box for current node

	// -------------------------------------------------------------------------
	// void sl_midpt_split(			// sliding midpoint split rule
	// 	int *pidx,						// point index
	// 	int n,							// number of points
	// 	const KD_Rect &bnds,			// bounding rect for cell
	// 	int &cut_dim,					// cutting dimension (return)
	// 	float &cut_val,					// cutting value (return)
	// 	int &num_low);					// num of pts on low side (return)
	// -------------------------------------------------------------------------
	float random_split(
		int* pidx,								// point index	
		int n,									// number of points
		float& cut_val,							// cutting value (return)
		int& num_low);							// num of pts on low side (return)    
	// -------------------------------------------------------------------------
	// float calc_spread(				// calc point spread along dim
	// 	int* pidx,						// point indices
	// 	int n,							// number of points
	// 	int d);							// dimension to check

	// // -------------------------------------------------------------------------
	// float calc_mean(                // calc mean
	// 	int *pidx,                      // point indices
	// 	int n,                          // number of points
	// 	int d,                          // dimension to check
	// 	float &min,                     // min value (return)
	// 	float &max);                    // max value (return)

	// // -------------------------------------------------------------------------
	// void calc_statistics(           // calc mean, median and variance along dim
	// 	int *pidx,						// point indices
	// 	int n,							// number of points
	// 	int d,							// dimension to check
	// 	float &mean,                    // mean value (return)
	// 	float &median,					// median value (return)
	// 	float &variance,				// variance (return)
	// 	float &min,						// min value (return)
	// 	float &max);					// max value (return)

	// // -------------------------------------------------------------------------
	// void calc_minmax(				// calc min and max value along dim
	// 	int *pidx,						// point indices
	// 	int n,							// number of points
	// 	int d,							// dimension to check
	// 	float &min,						// min value (return)
	// 	float &max);					// max value (return)

	// // -------------------------------------------------------------------------
	// void plane_split(				// split points by a plane
	// 	int *pidx,						// point index
	// 	int n,							// number of points
	// 	int d,							// cutting dimension
	// 	float cv,						// cutting value
	// 	int &br1,						// 1st break (values < cv) (return)
	// 	int &br2);						// 2nd break (values = cv) (return)

	// // -------------------------------------------------------------------------
	// float calc_box_dist(			// compute distance from point to box
	// 	const float *q,					// query point
	// 	const float *low,				// low point of box
	// 	const float *high,				// high point of box
	// 	int dim);						// dimensionality
};

#endif
