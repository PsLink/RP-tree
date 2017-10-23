#ifndef __DEF_H
#define __DEF_H

// -----------------------------------------------------------------------------
//  Typedefs
// -----------------------------------------------------------------------------
typedef char Block[];

// -----------------------------------------------------------------------------
//  Macros
// -----------------------------------------------------------------------------
#define MIN(a, b)	(((a) < (b)) ? (a) : (b))
#define MAX(a, b)	(((a) > (b)) ? (a) : (b))
#define POW(x)		((x) * (x))
#define SUM(x, y)	((x) + (y))
#define DIFF(x, y)	((y) - (x))
#define SWAP(x, y)	{int tmp=x; x=y; y=tmp;}

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

// -----------------------------------------------------------------------------
//  Constants
// -----------------------------------------------------------------------------
const int MAXK = 100;
const long long MAXMEMORY = 8589934591;	// max memory, 8 GB

const float E  = 2.7182818F;
const float PI = 3.141592654F;
const float FLOATZERO = 1e-6F;

const float MAXREAL = 3.402823466e+38F;
const float MINREAL = -MAXREAL;
const int   MAXINT  = 2147483647;
const int   MININT  = -MAXINT;

const int SIZEINT   = (int) sizeof(int);
const int SIZECHAR  = (int) sizeof(char);
const int SIZEFLOAT = (int) sizeof(float);
const int SIZEBOOL  = (int) sizeof(bool);

const int BFHEAD_LENGTH = SIZEINT * 2;
const int INDEX_SIZE_LEAF_NODE = 64; // index size of leaf node of b-tree

const char	DIVIDE_INDEX[] = "divide_indices";
const char	DIVIDE_DATA[] = "divide_data";

#endif
