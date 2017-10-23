#include "headers.h"

// -----------------------------------------------------------------------------
//	KD_Tree: structure for approximate and exact nearest neighbor search
// -----------------------------------------------------------------------------
KD_Tree::KD_Tree(						// constructor
	float** pts,							// point array
	int n,									// number of points
	int d,									// dimensionality
	int kd_leaf_size)						// leaf size of kd-tree
{
	pts_ = pts;							// init <pts>
	n_pts_ = n;							// init <n_pts>
	dim_ = d;							// init <dim>
	kd_leaf_size_ = kd_leaf_size;		// init <kd_leaf_size>

	pidx_ = new int[n_pts_];			// init <pidx>
	for (int i = 0; i < n_pts_; i++) {
		pidx_[i] = i;
	}
										// init <root_>
	root_ = rkd_tree(pidx_, n_pts_);
}

// -----------------------------------------------------------------------------
KD_Tree::~KD_Tree()						// destructor
{
	if (root_ != NULL) {
		delete root_; root_ = NULL;
	}

	if (pidx_ != NULL) {
		delete[] pidx_; pidx_ = NULL;
	}
}

// -----------------------------------------------------------------------------
KD_Node* KD_Tree::rkd_tree(				// recursive build kd-tree
	int* pidx,								// point index
	int n)									// number of points
	// KD_Rect& bnd_box)						// bounding box for current node
{
	if (n <= kd_leaf_size_) {			// leaf node
		return new KD_Leaf(pts_, dim_, n, pidx);
	}
	else {								// split node
		// int cut_dim;					// cutting dimension
		float cut_val;					// cutting value
		int num_low;					// number of lower side
		KD_Node* left;					// left child
		KD_Node* right;					// right child

										// make split
		// sl_midpt_split(pidx, n, bnd_box, cut_dim, cut_val, num_low);
		random_split(pidx,n,cut_val, num_low);
										// save bounds for cutting dimension
		// float low_val = bnd_box.low_[cut_dim];
		// float high_val = bnd_box.high_[cut_dim];

										// recursive build left son
		// bnd_box.high_[cut_dim] = cut_val;// from pidx[0...num_low-1]
		// left = rkd_tree(pidx, num_low, bnd_box);
		// bnd_box.high_[cut_dim] = high_val;// restore bound
		left = rkd_tree(pidx, num_low);

										// recursive build right son
		// bnd_box.low_[cut_dim] = cut_val;// from pidx[num_low...n-1]
		// right = rkd_tree(pidx + num_low, n - num_low, bnd_box);
		// bnd_box.low_[cut_dim] = low_val;// restore bound
		right = rkd_tree(pidx + num_low, n - num_low);

										// create new split node
		return new KD_Split(pts_, dim_, 0, cut_val, 0, 0, left, right);
	}
}

// -----------------------------------------------------------------------------
float KD_Tree::random_split(               	// calc mean
	int* pidx,								// point index
	int n,									// number of points
	float& cut_val,							// cutting value (return)
	int& num_low)							// num of pts on low side (return)                             
{
	// float mean = 0.0f;
	// min = pts_[pidx[0]][d];
	// max = pts_[pidx[0]][d];


	float *randomVec = new float[dim_];
	float *dotProduct = new float[n];
	float *arr = new float[n];
	float median;

	// for (int i = 0; i<dim_; i++) randomVec[i] = uniform(-1,1);
	for (int i = 0; i<dim_; i++) randomVec[i] = gaussian(0,1);
	for (int i = 0; i<n; i++) dotProduct[i] = dotMultiply(randomVec,pts_[pidx[i]],dim_);

	for (int i = 0; i < n; i++) arr[i] = dotProduct[i];

	sort(arr, arr + n);
	if (n % 2 != 0) {
		median = arr[n / 2];
	}
	else {
		median = (arr[n / 2 - 1] + arr[n / 2]) / 2;
	}

	cut_val = median;

	float cv = median; // cut value

	int left = 0;
	int right = n - 1;
	int br1,br2;

	for (;;) {							// partition pa[0..n-1] about cv
		while (left < n && dotProduct[left] < cv) left++;
		while (right >= 0 && dotProduct[right] >= cv) right--;
		if (left > right) break;

		SWAP(pidx[left], pidx[right]);	// swap point index
		SWAP(dotProduct[left],dotProduct[right]);
		left++; right--;
	}
	br1 = left;							// pa[0..br1-1] < cv <= pa[br1..n-1]

	right = n - 1;
	for (;;) {							// partition pa[br1..n-1] about cv
		while (left < n && dotProduct[left] <= cv) left++;
		while (right >= 0 && dotProduct[right] > cv) right--;
		if (left > right) break;

		SWAP(pidx[left], pidx[right]);	// swap point index
		SWAP(dotProduct[left],dotProduct[right]);
		left++; right--;
	}
	br2 = left;

	if (br1 > n / 2) num_low = br1;
	else if (br2 < n / 2) num_low = br2;
	else num_low = n / 2;
}

// -----------------------------------------------------------------------------
void KD_Tree::traversal(
	vector<int> &block_size,			// leaf size (return)
	int *object_id)						// object id with leaf order (return)
{
	for (int i = 0; i < n_pts_; i++) {
		object_id[i] = pidx_[i];
	}
	root_->traversal(block_size);
}


// // -----------------------------------------------------------------------------
// void KD_Tree::sl_midpt_split(			// sliding midpoint split rule
// 	int* pidx,								// point index
// 	int n,									// number of points
// 	const KD_Rect& bnds,					// bounding rect for cell
// 	int& cut_dim,							// cutting dimension (return)
// 	float& cut_val,							// cutting value (return)
// 	int& num_low)							// num of pts on low side (return)
// {
// 	float max_var = MINREAL;
// 	float cut_min = -1.0f;
// 	float cut_max = -1.0f;
// 	cut_val = -1.0f;
// 	cut_dim = -1;

// 	float mean, median, variance, min, max;
// 	for (int i = 0; i < dim_; i++) {
// 		calc_statistics(pidx, n, i, mean, median, variance, min, max);

// 		if (variance > max_var) {
// 			max_var = variance;
// 			cut_val = median;//mean; //
// 			cut_dim = i;
// 			cut_min = min;
// 			cut_max = max;
// 		}
// 	}

// 	int br1, br2;						// permute points accordingly
// 	plane_split(pidx, n, cut_dim, cut_val, br1, br2);

// 	//--------------------------------------------------------------------------
// 	//	on return:		pa[0..br1-1]   <  cut_val
// 	//					pa[br1..br2-1] == cut_val
// 	//					pa[br2..n-1]   >  cut_val
// 	//
// 	//	we can set num_low to any value in the range [br1..br2]
// 	//--------------------------------------------------------------------------
// 	if (cut_val < cut_min) num_low = 1;
// 	else if (cut_val > cut_max) num_low = n - 1;
// 	else if (br1 > n / 2) num_low = br1;
// 	else if (br2 < n / 2) num_low = br2;
// 	else num_low = n / 2;
// }



// -----------------------------------------------------------------------------
// void KD_Tree::plane_split(				// split points by a plane
// 	int* pidx,								// point index
// 	int n,									// number of points
// 	int d,									// cutting dimension
// 	float cv,								// cutting value
// 	int& br1,								// 1st break (values < cv) (return)
// 	int& br2)								// 2nd break (values = cv) (return)
// {
// 	int left = 0;
// 	int right = n - 1;
// 	for (;;) {							// partition pa[0..n-1] about cv
// 		while (left < n && pts_[pidx[left]][d] < cv) left++;
// 		while (right >= 0 && pts_[pidx[right]][d] >= cv) right--;
// 		if (left > right) break;

// 		SWAP(pidx[left], pidx[right]);	// swap point index
// 		left++; right--;
// 	}
// 	br1 = left;							// pa[0..br1-1] < cv <= pa[br1..n-1]

// 	right = n - 1;
// 	for (;;) {							// partition pa[br1..n-1] about cv
// 		while (left < n && pts_[pidx[left]][d] <= cv) left++;
// 		while (right >= 0 && pts_[pidx[right]][d] > cv) right--;
// 		if (left > right) break;

// 		SWAP(pidx[left], pidx[right]);	// swap point index
// 		left++; right--;
// 	}
// 	br2 = left;							// pa[br1..br2-1] = cv < pa[br2..n-1]
// }

// // -------------------------------------------------------------------------
// void KD_Tree::calc_statistics(			// calc median and variance value
// 	int *pidx,								// point indices
// 	int n,									// number of points
// 	int d,									// dimension to check
// 	float &mean,                            // mean value (return)
// 	float &median,							// median value (return)
// 	float &variance,						// variance (return)
// 	float &min,								// min value (return)
// 	float &max)								// max value (return)
// {
// 	// ---------------------------------------------------------------------
// 	//  calc mean, min, and max
// 	// ---------------------------------------------------------------------
// 	float *arr = new float[n];
// 	float val = pts_[pidx[0]][d];
// 	arr[0] = val;
// 	min = val;
// 	max = val;
// 	mean = val;
// 	for (int i = 1; i < n; i++) {
// 		val = pts_[pidx[i]][d];
// 		arr[i] = val;
// 		mean += val;

// 		if (val < min) min = val;
// 		else if (val > max) max = val;
// 	}
// 	mean /= n;

// 	// ---------------------------------------------------------------------
// 	//  calc median
// 	// ---------------------------------------------------------------------
// 	sort(arr, arr + n);
// 	if (n % 2 != 0) {
// 		median = arr[n / 2];
// 	}
// 	else {
// 		median = (arr[n / 2 - 1] + arr[n / 2]) / 2;
// 	}

// 	// ---------------------------------------------------------------------
// 	//  calc variance
// 	// ---------------------------------------------------------------------
// 	variance = 0.0f;
// 	for (int i = 0; i < n; i++) {
// 		float diff = pts_[pidx[i]][d] - mean;
// 		variance += diff * diff;
// 	}
// 	variance /= n;

// 	// ---------------------------------------------------------------------
// 	//  release space
// 	// ---------------------------------------------------------------------
// 	delete[] arr; arr = NULL;
// }