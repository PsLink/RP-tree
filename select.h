#ifndef __SELECT_H
#define __SELECT_H

class MinK_List;

// -----------------------------------------------------------------------------
//  Basic Data Structure for Approximate Nearest Neighbor Search
// -----------------------------------------------------------------------------
struct Score_Pair {
	int obj_;
	float score_;
};

// -----------------------------------------------------------------------------
struct Order_Pair {
	int block_id_;
	int time_;
};

// -----------------------------------------------------------------------------
//  Comparison Function for qsort
// -----------------------------------------------------------------------------
int score_compar(
	const void* xv,
	const void* yv);

// -----------------------------------------------------------------------------
int order_compar(
	const void* xv,
	const void* yv);

// -----------------------------------------------------------------------------
int kd_tree(						// find top-k mip using qalsh
	int n,								// number of data points
	int d,								// dimensionality
	int kd_leaf_size,					// leaf size of kd-tree
	float **data,						// dataset
	char *output_folder,				// output folder
	vector<int> &block_size,			// size of each block (return) 
	int *object_id);					// object id (return)

// -----------------------------------------------------------------------------
void drusilla_select(				// build index of DrusillaSelect
	int d,								// dimensionality
	int l,								// number of projections
	int m,								// number of candidates for each proj
	vector<int> &block_size,			// size of each block
	int *object_id,						// object id with block order
	float **data,						// original data objects
	char *output_folder,				// output folder
	int *sample_id,						// sample data objects' id (return)
	float **sample_data);				// sample data objects (return)

// -----------------------------------------------------------------------------
//  shift data objects to their centroid and make a copy of data after shifting
// -----------------------------------------------------------------------------
int calc_shift_data(				// calc <shift_data>
	int n,								// number of data points
	int d,								// dimensionality
	int *object_id,						// object id (one block)
	float **data,						// original data objects
	float **shift_data);				// shift data objects (return)

// -----------------------------------------------------------------------------
void drusilla_build(				// build index of DrusillaSelect
	int n,								// number of objects
	int d,								// dimensionality
	int l,								// number of projections
	int m,                              // number of candidates for each proj
	float **shift_data,					// shift data
	int *object_id,						// object id
	int *sample_id);					// sample objects id (return)

// -----------------------------------------------------------------------------
void get_block_order(				// get block order
	int num_blocks,						// number of blocks
	int nb,								// number of blocks for search
	MinK_List *list,					// sample top-t results
	unordered_map<int, int> &block_id,	// block id
	vector<int> &block_order);			// block order (return)

#endif
