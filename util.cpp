#include "headers.h"


// -----------------------------------------------------------------------------
//  Global variables
// -----------------------------------------------------------------------------
long long g_memory = 0;


// -----------------------------------------------------------------------------
//  Utility functions
// -----------------------------------------------------------------------------
float dotMultiply(
	float *v1,
	float *v2,
	int length)
{
	float val = 0;
	for (int i=0; i<length; i++)
		val += v1[i]*v2[i];
	return val;
}

// -----------------------------------------------------------------------------
int compfloats(						// compare two real values 
	float v1,							// 1st value (type float)
	float v2)							// 2nd value (type float)
{
	if (v1 - v2 < -FLOATZERO) return -1;
	else if (v1 - v2 > FLOATZERO) return 1;
	else return 0;
}

// -----------------------------------------------------------------------------
void error(							// display an error message
	char *msg,							// message
	bool is_exit)						// whether exit
{
	printf("%s\n", msg);
	if (is_exit) exit(1);
}

// -----------------------------------------------------------------------------
void get_lp_filename(				// get file name of L_p norm
	float p,							// the p value of L_p norm
	char* fname)						// file name (return)
{
	char c[20];

	sprintf(c, "%.2f", p);
	strcat(fname, "L");
	strcat(fname, c);
}

// -----------------------------------------------------------------------------
int check_mem()						// check memory is enough
{
	if (g_memory > MAXMEMORY) {		// default maximum memory is 1 GB
		printf("I am going to need around %.2f MB memory.\n", 
			(float) g_memory / (1024.0f * 1024.0f));

		printf("Can you afford it (y/n)?\n");
		char c = getchar();			// ask for more memory
		getchar();
		if (c != 'y' && c != 'Y') {
			return 1;				// fail to return
		}
	}
	return 0;
}

// -------------------------------------------------------------------------
//  Check whether the directory exists. If the directory does not exist, we 
//  create the directory for each folder.
// -------------------------------------------------------------------------
int create_dir(
	char* path)
{
#ifdef LINUX_
	int len = (int)strlen(path);	// create directory under Linux
	for (int i = 0; i < len; i++) {
		if (path[i] == '/') {
			char ch = path[i + 1];
			path[i + 1] = '\0';
			// check whether the directory exists
			int ret = access(path, F_OK);
			if (ret != 0) {			// create the directory
				ret = mkdir(path, 0755);
				if (ret != 0) {
					printf("Could not create directory %s\n", path);
					error("write_data_new_form error\n", true);
					return 1;		// fail to return
				}
			}
			path[i + 1] = ch;
		}
	}
#else
	int len = (int)strlen(path);	// create directory under Windows
	for (int i = 0; i < len; i++) {
		if (path[i] == '/') {
			char ch = path[i + 1];
			path[i + 1] = '\0';
			// check whether the directory exists
			int ret = _access(path, 0);
			if (ret != 0) {			// create the directory
				ret = _mkdir(path);
				if (ret != 0) {
					printf("Could not create directory %s\n", path);
					error("check directory error\n", true);
					return 1;		// fail to return
				}
			}
			path[i + 1] = ch;
		}
	}
#endif
	return 0;
}


// -----------------------------------------------------------------------------
float calc_lp_dist(					// calc L_p distance (type float)
	float  p,							// the p value of Lp norm, p in (0, 2]
	float* vec1,						// 1st point
	float* vec2,						// 2nd point
	int    dim)							// dimension
{
	float diff = 0.0f;
	float ret = 0.0f;

	if (compfloats(p, 0.5f) == 0) {	// calc L_1/2 norm
		for (int i = 0; i < dim; i++) {
			diff = fabs(vec1[i] - vec2[i]);
			ret += sqrt(diff);
		}
		return ret * ret;
	}								// calc L_1 norm
	else if (compfloats(p, 1.0f) == 0) {
		for (int i = 0; i < dim; i++) {
			ret += fabs(vec1[i] - vec2[i]);
		}
		return ret;
	}								// calc L2 norm
	else if (compfloats(p, 2.0f) == 0) {
		for (int i = 0; i < dim; i++) {
			diff = vec1[i] - vec2[i];
			ret += diff * diff;
		}
		return sqrt(ret);
	}
	else {							// calc other Lp norm (general way)
		for (int i = 0; i < dim; i++) {
			diff = fabs(vec1[i] - vec2[i]);
			ret += pow(diff, p);
		}
		return pow(ret, 1.0f / p);
	}
}


// -----------------------------------------------------------------------------
//  Read the original dataset (or query set) from disk
// -----------------------------------------------------------------------------
int read_set(						// read (data or query) set from disk
	int n,								// number of data points
	int d,								// dimensionality
	char* set,							// address of dataset
	float** points)						// data or queries (return)
{
	int i = 0;
	int j = 0;
	FILE* fp = NULL;

	fp = fopen(set, "r");			// open data file
	if (!fp) {
		printf("I could not open %s.\n", set);
		return 1;
	}

	i = 0;
	while (!feof(fp) && i < n) {	// read data file
		// fscanf(fp, "%d", &j);
		for (j = 0; j < d; j++) {
			fscanf(fp, " %f", &points[i][j]);
		}
		fscanf(fp, "\n");

		i++;
	}
	if (!feof(fp) && i == n) {		// check the size of set
		error("The size of set is larger than you input\n", false);
	}
	else if (feof(fp) && i < n) {
		printf("Set the size of dataset to be %d. ", i);
		error("And try again\n", true);
	}
	fclose(fp);						// close data file
	return 0;
}

// -----------------------------------------------------------------------------
//  Write the data set in new format to the disk
// -----------------------------------------------------------------------------
int write_data_new_form(			// write dataset with new format
	int n,								// cardinality
	int d,								// dimensionality
	int B,								// page size
	float** data,						// data set
	char* output_path)					// output path
{
									// number of data in one data file
	int num = (int) floor((float) B / (d * SIZEFLOAT));
									// total number of data file
	int total_file = (int) ceil((float) n / num);
	if (total_file == 0) {
		return 1;					// fail to return
	}

	// -------------------------------------------------------------------------
	//  Check whether the directory exists. If the directory does not exist, we 
	//  create the directory for each folder.
	// -------------------------------------------------------------------------
	char data_path[200];
	strcpy(data_path, output_path);
	strcat(data_path, "data/");
	create_dir(data_path);
	
	// -------------------------------------------------------------------------
	//  Write data of qalsh
	// -------------------------------------------------------------------------
	char fname[300];
	char* buffer = new char[B];		// allocate one page size
	for (int i = 0; i < B; i++) {
		buffer[i] = 0;
	}
	g_memory += SIZECHAR * B;

	int left  = 0;
	int right = 0;
	for (int i = 0; i < total_file; i++) {
									// get file name of data
		get_data_filename(i, data_path, fname);

		left = i * num;
		right = left + num;
		if (right > n) right = n;	// write data to buffer
		write_data_to_buffer(d, left, right, data, buffer);

									// write one page of data to disk
		if (write_buffer_to_page(B, fname, buffer) == 1) {
			error("write_data_new_form error to write a page", true);
		}
	}

	// -------------------------------------------------------------------------
	//  Release space
	// -------------------------------------------------------------------------
	delete[] buffer; buffer = NULL;
	g_memory -= SIZECHAR * B;
	
	return 0;
}

// -----------------------------------------------------------------------------
void get_data_filename(				// get file name of data
	int file_id,						// data file id
	char* data_path,					// path to store data in new format
	char* fname)						// file name of data (return)
{
	char c[20];

	strcpy(fname, data_path);
	sprintf(c, "%d", file_id);
	strcat(fname, c);
	strcat(fname, ".data");
}

// -----------------------------------------------------------------------------
void write_data_to_buffer(			// write data to buffer
	int d,								// dimensionality
	int left,							// left data id
	int right,							// right data id
	float** data,						// data set
	char* buffer)						// buffer to store data
{
	int c = 0;
	for (int i = left; i < right; i++) {
		for (int j = 0; j < d; j++) {
			memcpy(&buffer[c], &data[i][j], SIZEFLOAT);
			c += SIZEFLOAT;
		}
	}
}

// -----------------------------------------------------------------------------
int write_buffer_to_page(			// write buffer to one page
	int B,								// page size
	char* fname,						// file name of data
	char* buffer)						// buffer to store data
{
	if (fname == NULL || buffer == NULL) return 1;

	FILE* fp = NULL;
	fp = fopen(fname, "wb");		// open data file to write
	if (!fp) {
		printf("I could not create %s.\n", fname);
		return 1;					// fail to return
	}

	fwrite(buffer, B, 1, fp);
	fclose(fp);
	return 0;
}

// -----------------------------------------------------------------------------
//  Read data in new format from disk
// -----------------------------------------------------------------------------
int read_data(						// read data from page
	int id,								// index of data
	int d,								// dimensionality
	int B,								// page size
	float* data,						// real data (return)
	char* output_path)					// output path
{
	// -------------------------------------------------------------------------
	//  Get file name of data
	// -------------------------------------------------------------------------
	char fname[300];
	char data_path[300];

	strcpy(data_path, output_path);
	strcat(data_path, "data/");
									// number of data in one data file
	int num = (int) floor((float) B / (d * SIZEFLOAT));
									// data file id
	int file_id = (int) floor((float) id / num);

	get_data_filename(file_id, data_path, fname);

	// -------------------------------------------------------------------------
	//  Read buffer (one page of data) in new format from disk
	// -------------------------------------------------------------------------
	char* buffer = new char[B];		// allocate one page size
	g_memory += SIZECHAR * B;
	for (int i = 0; i < B; i++) {
		buffer[i] = 0;
	}
	if (read_buffer_from_page(B, fname, buffer) == 1) {
		error("read_data() error to read a page", true);
	}

	// -------------------------------------------------------------------------
	//  Read data from buffer
	// -------------------------------------------------------------------------
	int index = id % num;
	read_data_from_buffer(index, d, data, buffer);

	delete[] buffer; buffer = NULL;
	g_memory -= SIZECHAR * B;

	return 0;
}

// -----------------------------------------------------------------------------
int read_buffer_from_page(			// read buffer from page
	int B,								// page size
	char* fname,						// file name of data
	char* buffer)						// buffer to store data
{
	if (fname == NULL || buffer == NULL) return 1;

	FILE* fp = NULL;
	fp = fopen(fname, "rb");
	if (!fp) {
		printf("read_buffer_from_page could not open %s.\n", fname);
		return 1;					// fail to return
	}

	fread(buffer, B, 1, fp);
	fclose(fp);
	return 0;
}

// -----------------------------------------------------------------------------
void read_data_from_buffer(			// read data from buffer
	int index,							// index of data in buffer
	int d,								// dimensionality
	float* data,						// data set
	char* buffer)						// buffer to store data
{
	int c = index * d * SIZEFLOAT;
	for (int i = 0; i < d; i++) {
		memcpy(&data[i], &buffer[c], SIZEFLOAT);
		c += SIZEFLOAT;
	}
}
