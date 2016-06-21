#pragma once

#define GAUSS_N 3

class CLineExtractor
{
public:
	static CLineExtractor* GetInstance()
    {
        static CLineExtractor* instance = new CLineExtractor();
        return instance;
    }

    ~CLineExtractor();

	int Extract( int* pnNumPt, double (*pdData)[500][2], double (*pdCoeff)[3] );

	void Set( double (*pdCoeff)[3] );

	double DistanceWrtLane( double dPosX, double dPosY, int nLane, int& nSide );
	

private:
	CLineExtractor();

	void gauss(double a[GAUSS_N][GAUSS_N+1], double p[GAUSS_N]);
	void approximate( int nDataNum, double dArray[][2], double* co );
	void deleteCoefficientData( void );

	double** m_ppdCoefficient;
};