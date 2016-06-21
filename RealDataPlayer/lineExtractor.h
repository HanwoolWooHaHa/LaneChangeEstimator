#pragma once

#define GAUSS_NUM 3

class CLineExtractor
{
public:
	static CLineExtractor* GetInstance()
    {
        static CLineExtractor* instance = new CLineExtractor();
        return instance;
    }

    ~CLineExtractor();

	int Extract( int nLine, int nNumMarks, double** ppdData, double* pdCoeff );

	/**
	* @fn SetCoefficients
	* @param pdCoeff : 近似曲線の係数
	* @return void
	* @remark ファイルから読み込んだ白線の係数をセットする．Extract()とは同時に使われない．
	*/
	void SetCoefficients( double (*pdCoeff)[3] );

	double DistanceWrtLine( double dPosX, double dPosY, int nLane, int& nSide );
	

private:
	CLineExtractor();

	void gauss(double a[GAUSS_NUM][GAUSS_NUM + 1], double p[GAUSS_NUM]);
	void approximate( int nDataNum, double** dArray, double* co );
	void deleteCoefficientData( void );

	double** m_ppdCoefficient;
};