#include "headers.h"
// Build a KD tree for clustering
// -----------------------------------------------------------------------------
int main(int nargs, char **args)
{
	// -------------------------------------------------------------------------
	//  Read data set
	// -------------------------------------------------------------------------

	int n = 1000000, d=128;
	clock_t startTime = (clock_t)-1;
	clock_t endTime = (clock_t)-1;
	
	startTime = clock();
	float** data = new float*[n];
	for (int i = 0; i < n; i++) data[i] = new float[d];
	if (read_set(n, d, "data.txt", data) == 1) {
		error("Reading Dataset Error!\n", true);
	}
	endTime = clock();
	printf("Read Dataset: %.6f Seconds\n\n",
		((float)endTime - startTime) / CLOCKS_PER_SEC);

	// -------------------------------------------------------------------------
	//  KD tree
	// -------------------------------------------------------------------------	

	vector<int> block_size;
	int *object_id = new int[n];
	int kd_leaf_size = 1000;

	startTime = clock();			// construct KD-Tree

	KD_Tree* tree = new KD_Tree(data, n, d, kd_leaf_size);
	tree->traversal(block_size, object_id);

	endTime = clock();
	float indexTime = ((float)endTime - startTime) / CLOCKS_PER_SEC;
	printf("KD-Tree Construction = %.3f s\n\n", indexTime);

	FILE* fp = NULL;
	char output_set[300];			// output file
	// strcpy(output_set, output_folder);
	strcat(output_set, "RPTree.out");

	fp = fopen(output_set, "w");	// create output file
	if (!fp) error("Could not create output file.", true);
	
	// fprintf(fp, "indexing time = %f\n", indexTime);
	int size = (int) block_size.size();
	// fprintf(fp, "total number of blocks = %d\n", size);
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
