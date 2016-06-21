#pragma once

#include "constant.h"

class CMathAlg
{
public:
	static CMathAlg* GetInstance()
    {
        static CMathAlg* instance = new CMathAlg();
        return instance;
    }

    ~CMathAlg();

	void Sort( int num, int mode, double dArray[][3] );
	void Approximate( int nNum, double dArray[][3], double* k ); // ‹ß—‹Èü‚ğ‹‚ß‚éŠÖ”
	double DistanceWrtLane( double x, double y, double resolution, double* coeff );
	void Transformation( double ox, double oy, double ang, double px, double py, double* rx, double* ry );
	
private:
	CMathAlg();

	void gauss(double a[GAUSS_N][GAUSS_N+1], double p[GAUSS_N]);
};