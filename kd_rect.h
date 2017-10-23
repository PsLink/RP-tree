#ifndef __KD_RECT_H
#define __KD_RECT_H

// -----------------------------------------------------------------------------
//	KD_Rect: orthogonal rectangle for bounding rectangle of kd-tree
// -----------------------------------------------------------------------------
class KD_Rect {
public:
	float *low_;					// rectangle lower bound
	float *high_;					// rectangle upper bound

	KD_Rect(						// constructor
		int dim,						// dimensionality
		float l = 0,					// boundary, default is zero
		float h = 0);					// boundary

	KD_Rect(						// copy constructor
		int dim,						// dimensionality
		const KD_Rect &rect);			// rectangle to copy

	KD_Rect(						// construct from points
		int dim,						// dimensionality
		float *low,						// low point
		float *high);					// high point

	~KD_Rect();						// destructor

	bool inside(					// whether a point inside rectangle
		int dim,						// dimensionality
		float *point);					// one point
};

#endif