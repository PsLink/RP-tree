#include "headers.h"


// -----------------------------------------------------------------------------
//	KD_Leaf: leaf node of kd-tree.
// -----------------------------------------------------------------------------
KD_Leaf::KD_Leaf(					// constructor
	float **pts,						// data points
	int dim,							// dimension of data points
	int n,								// num of points
	int *bkt)                           // bucket
{
	pts_ = pts;						// init <pts_>
	dim_ = dim;						// init <dim_>
	n_pts_ = n;						// init <n_pts_>
	bkt_ = bkt;						// init <bkt_>
}

// -----------------------------------------------------------------------------
KD_Leaf::~KD_Leaf()					// destructor
{
}

// -----------------------------------------------------------------------------
void KD_Leaf::search(				// tree search
	float* query,						// query point
	float box_dist,						// box distance to query
	float ratio,						// approximation ratio
	MinK_List* list)					// top-k nn list
{
	float dist, t;
	float* point;
	for (int i = 0; i < n_pts_; i++) {
		point = pts_[bkt_[i]];
		dist = 0.0F;

		for (int j = 0; j < dim_; j++) {
			t = point[j] - query[j];
			dist = SUM(dist, POW(t));
		}
		list->insert(dist, bkt_[i]);
	}
}

// -----------------------------------------------------------------------------
void KD_Leaf::traversal(			// traversal kd-tree
	vector<int> &block_size)			// leaf size (return)
{
	block_size.push_back(n_pts_);
}


// -----------------------------------------------------------------------------
//	KD_Split: split node of kd-tree.
// -----------------------------------------------------------------------------
KD_Split::KD_Split(					// constructor
	float** pts,						// data points
	int dim,							// dimension of data pointss
	int cd,								// cutting dimension
	float cv,							// cutting value
	float lv,							// low bound in dim
	float hv,							// high bound in dim
	KD_Node* lc,						// left child
	KD_Node* rc)						// right child
{
	pts_ = pts;						// init <pts_>
	dim_ = dim;						// init <dim_>
	cut_dim_ = cd;					// init <cut_dim_>
	cut_val_ = cv;					// init <cut_val_>

	cd_bnds_[0] = lv;				// init <cd_bnds_>
	cd_bnds_[1] = hv;

	child_[0] = lc;					// init <child_>
	child_[1] = rc;
}

// -----------------------------------------------------------------------------
KD_Split::~KD_Split()				// destructor
{
	if (child_[0] != NULL) {
		delete child_[0]; child_[0] = NULL;
	}
	if (child_[1] != NULL) {
		delete child_[1]; child_[1] = NULL;
	}
}

// -----------------------------------------------------------------------------
void KD_Split::search(				// tree search
	float* query,						// query point
	float box_dist,						// box distance to query
	float ratio,						// approximation ratio
	MinK_List* list)					// top-k nn list
{
	float cut_diff = query[cut_dim_] - cut_val_;
	if (cut_diff < 0) {				// query on left of cutting plane
		child_[0]->search(query, box_dist, ratio, list);

		float box_diff = cd_bnds_[0] - query[cut_dim_];
		if (box_diff < 0) box_diff = 0;	// within bounds - ignore

										// distance to further child
		box_dist = box_dist + (cut_diff * cut_diff - box_diff * box_diff);
		//box_dist = SUM(box_dist, DIFF(POW(box_diff), POW(cut_diff)));

		// visit right child
		if (box_dist * ratio < list->max_key()) {
			child_[1]->search(query, box_dist, ratio, list);
		}
	}
	else {							// query on right of cutting plane
		child_[1]->search(query, box_dist, ratio, list);

		float box_diff = query[cut_dim_] - cd_bnds_[1];
		if (box_diff < 0) box_diff = 0;	// within bounds - ignore
										// distance to further child
		box_dist = box_dist + (cut_diff * cut_diff - box_diff * box_diff);
		//box_dist = SUM(box_dist, DIFF(POW(box_diff), POW(cut_diff)));

		// visit left child
		if (box_dist * ratio < list->max_key()) {
			child_[0]->search(query, box_dist, ratio, list);
		}
	}
}

// -----------------------------------------------------------------------------
void KD_Split::traversal(
	vector<int> &block_size)			// leaf size (return)
{
	child_[0]->traversal(block_size);
	child_[1]->traversal(block_size);
}
