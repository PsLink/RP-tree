#include "headers.h"

// -----------------------------------------------------------------------------
//  Descending order for <Score_Pair>
// -----------------------------------------------------------------------------
int score_compar(
	const void *xv,
	const void *yv)
{
	float x, y;

	x = ((Score_Pair *)xv)->score_;
	y = ((Score_Pair *)yv)->score_;

	if (x > y) return -1;
	else if (x < y) return 1;
	else return 0;
}

// -----------------------------------------------------------------------------
//  Descending order for <Order_Pair>
// -----------------------------------------------------------------------------
int order_compar(
	const void* xv,
	const void* yv)
{
	Order_Pair *x = (Order_Pair*)xv;
	Order_Pair *y = (Order_Pair*)yv;

	if (x->time_ > y->time_) {
		return -1;
	}
	else if (x->time_ < y->time_) {
		return 1;
	}
	else {
		if (x->block_id_ < y->block_id_) return -1;
		else if (x->block_id_ > y->block_id_) return 1;
		else return 0;
	}
}

// -----------------------------------------------------------------------------
int kd_tree(						// find top-k mip using qalsh
	int n,								// number of data points
	int d,								// dimensionality
	int kd_leaf_size,					// leaf size of kd-tree
	float **data,						// dataset
	char *output_folder,				// output folder
	vector<int> &block_size,			// size of each block (return) 
	int *object_id)						// object id (return)
{
	if (n <= 0 || d <= 0) {
		error("parameters error in function read_file.", true);
	}

	clock_t startTime = (clock_t)-1;
	clock_t endTime = (clock_t)-1;

	startTime = clock();			// construct KD-Tree
	KD_Tree* tree = new KD_Tree(data, n, d, kd_leaf_size);
	tree->traversal(block_size, object_id);
	endTime = clock();

	float indexTime = ((float)endTime - startTime) / CLOCKS_PER_SEC;
	printf("KD-Tree Construction = %.3f s\n\n", indexTime);

	FILE* fp = NULL;
	char output_set[300];			// output file
	strcpy(output_set, output_folder);
	strcat(output_set, "KDTree.out");

	fp = fopen(output_set, "w");	// create output file
	if (!fp) error("Could not create output file.", true);
	
	fprintf(fp, "indexing time = %f\n", indexTime);
	int size = (int) block_size.size();
	fprintf(fp, "total number of blocks = %d\n", size);
	for (int i = 0; i < size; i++) {
		fprintf(fp, "%d ", block_size[i]);
	}
	fprintf(fp, "\n");

	for (int i = 0; i < n; i++) {
		fprintf(fp, "%d\n", object_id[i]);
	}
	fclose(fp);

	delete tree; tree = NULL;
	return 0;
}

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
	float **sample_data)				// sample data objects (return)
{
	char fname[300];
	strcpy(fname, output_folder);
	strcat(fname, "DrusillaSelect.out");

	FILE* fp = fopen(fname, "w");
	if (!fp) {
		printf("I could not open %s.\n", fname);
		exit(1);
	}

	int num_blocks = (int) block_size.size();
	int sample_size = l * m;

	int start = 0;
	int sample_start = 0;
	int count = 0;

	fprintf(fp, "%d %d\n", num_blocks, num_blocks * sample_size);
	for (int i = 0; i < num_blocks; i++) {
		// -------------------------------------------------------------------------
		//  calculate shift data
		// -------------------------------------------------------------------------
		int n = block_size[i];
		if (sample_size > n) {
			error("DrusillaSelect: l * m > n.\n", true);
		}

		float **shift_data = new float*[n];
		for (int j = 0; j < n; j++) shift_data[j] = new float[d];
		calc_shift_data(n, d, object_id + start, data, shift_data);
		
		// -------------------------------------------------------------------------
		//  select sample data from each blcok 
		// -------------------------------------------------------------------------
		drusilla_build(n, d, l, m, shift_data, object_id + start, 
			sample_id + sample_start);

		for (int j = 0; j < sample_size; j++) {
			int id = sample_id[sample_start + j];			
			for (int z = 0; z < d; z++) {
				sample_data[count][z] = data[id][z];
			}
			count++;
			fprintf(fp, "%d %d\n", i, id);
		}

		// -------------------------------------------------------------------------
		//  update parameters and release space
		// -------------------------------------------------------------------------
		start += n;
		sample_start += sample_size;
		for (int j = 0; j < n; j++) {
			delete[] shift_data[j]; shift_data[j] = NULL;
		}
		delete[] shift_data; shift_data = NULL;
	}
	fclose(fp);
}

// -----------------------------------------------------------------------------
//  shift data objects to their centroid and make a copy of data after shifting
// -----------------------------------------------------------------------------
int calc_shift_data(				// calc <shift_data>
	int n,								// number of data points
	int d,								// dimensionality
	int *object_id,						// object id (one block)
	float **data,						// original data objects
	float **shift_data)					// shift data objects (return)
{
	// -------------------------------------------------------------------------
	//  calculate the mean center (centroid) of the data objects
	// -------------------------------------------------------------------------
	float* centroid = new float[d];
	for (int i = 0; i < d; i++) {
		centroid[i] = 0.0f;
	}
	for (int i = 0; i < n; i++) {
		int id = object_id[i];
		for (int j = 0; j < d; j++) {
			centroid[j] += data[id][j];
		}
	}
	for (int i = 0; i < d; i++) {
		centroid[i] /= (float)n;
	}

	// -------------------------------------------------------------------------
	//  make a copy of data objects which move to the centroid of data objects
	// -------------------------------------------------------------------------
	for (int i = 0; i < n; i++) {
		int id = object_id[i];
		for (int j = 0; j < d; j++) {
			shift_data[i][j] = data[id][j] - centroid[j];
		}
	}

	// -------------------------------------------------------------------------
	//  release space
	// -------------------------------------------------------------------------
	delete[] centroid; centroid = NULL;
	return 0;
}

// -----------------------------------------------------------------------------
void drusilla_build(				// build index of DrusillaSelect
	int n,								// number of objects
	int d,								// dimensionality
	int l,								// number of projections
	int m,                              // number of candidates for each proj
	float **shift_data,					// shift data
	int *object_id,						// object id
	int *sample_id)						// sample objects id (return)
{
	// -------------------------------------------------------------------------
	//  calculate the norm of the objects and find the max norm
	// -------------------------------------------------------------------------
	float *norm = new float[n];
	int max_obj = -1;
	float max_norm = -1.0f;

	for (int i = 0; i < n; i++) {
		norm[i] = 0.0f;
		for (int j = 0; j < d; j++) {
			norm[i] += (shift_data[i][j] * shift_data[i][j]);
		}
		norm[i] = sqrt(norm[i]);

		if (norm[i] > max_norm) {
			max_norm = norm[i];
			max_obj = i;
		}
	}

	float *projection = new float[d];
	float *offset     = new float[n];
	float *distortion = new float[n];
	Score_Pair *spair = new Score_Pair[n];

	for (int i = 0; i < l; i++) {
		// ---------------------------------------------------------------------
		//  select the projection vector with largest norm and normalize it
		// ---------------------------------------------------------------------
		for (int j = 0; j < d; j++) {
			projection[j] = shift_data[max_obj][j] / norm[max_obj];
		}

		// ---------------------------------------------------------------------
		//  calculate offsets and distortions
		// ---------------------------------------------------------------------
		for (int j = 0; j < n; j++) {
			if (norm[j] > 0) {
				offset[j] = 0.0f;
				for (int k = 0; k < d; k++) {
					offset[j] += (shift_data[j][k] * projection[k]);
				}

				distortion[j] = 0.0f;
				for (int k = 0; k < d; k++) {
					float x = shift_data[j][k] - offset[j] * projection[k];
					distortion[j] += (x * x);
				}

				spair[j].obj_ = j;
				spair[j].score_ = offset[j] * offset[j] - distortion[j];
			}
			else {
				spair[j].obj_ = -1;
				spair[j].score_ = MINREAL;
			}
		}

		// ---------------------------------------------------------------------
		//  collect the objects that are well-represented by this projection
		// ---------------------------------------------------------------------
		qsort(spair, n, sizeof(Score_Pair), score_compar);
		for (int j = 0; j < m; j++) {
			int obj = spair[j].obj_;

			sample_id[i * m + j] = object_id[obj];
			norm[obj] = -1.0f;
		}

		// ---------------------------------------------------------------------
		//  find the next largest norm and the corresponding object
		// ---------------------------------------------------------------------
		max_obj = -1;
		max_norm = -1.0f;
		for (int j = 0; j < n; j++) {
			if (norm[j] > max_norm) {
				max_norm = norm[j];
				max_obj = j;
			}
		}
	}

	delete[] norm; norm = NULL;
	delete[] projection; projection = NULL;
	delete[] offset; offset = NULL;
	delete[] distortion; distortion = NULL;
	delete[] spair; spair = NULL;
}

// -----------------------------------------------------------------------------
void get_block_order(				// get block order
	int num_blocks,						// number of blocks
	int nb,								// number of blocks for search
	MinK_List *list,					// sample top-t results
	unordered_map<int, int> &block_id,	// block id
	vector<int> &block_order)			// block order (return)
{
	if (nb > num_blocks) {
		error("DrusillaSelect: nb > num_blocks.\n", true);
		return;
	}

	Order_Pair *pair = new Order_Pair[num_blocks];
	for (int i = 0; i < num_blocks; i++) {
		pair[i].block_id_ = i;
		pair[i].time_ = 0;
	}

	int size = list->size();
	for (int i = 0; i < size; i++) {
		int id = block_id[list->ith_smallest_id(i)];
		pair[id].time_++;
	}

	qsort(pair, num_blocks, sizeof(Order_Pair), order_compar);

	//int mean_time = size / num_blocks;
	for (int i = 0; i < nb; i++) {
		//if (pair[i].time < mean_time) break;

		block_order.push_back(pair[i].block_id_);
		//printf("block id = %d, time = %d\n", pair[i].block_id, pair[i].time);
	}
	//printf("\n");

	delete[] pair; pair = NULL;
}
