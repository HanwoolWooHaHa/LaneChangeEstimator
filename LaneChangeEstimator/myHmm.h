#pragma once

#include "methodology.h"

struct stHMM;
class CBaumWelch;

/**
* @class	CMyHmm
* @author	Hanwool Woo
* @version	1.10
* @date	Creation date: 2016/02/01 \n
* 			    Last revision date: 2016/02/01 HanwoolWoo
* @brief	this class is the implementation of Hidden Markov Model
*/

class CMyHmm : public CMethodology
{
public:
	CMyHmm();
	~CMyHmm();

	virtual void Train( int nType );
	virtual void Test( void );

private:
	enum { STATE_KEEPING=1, STATE_CHANGING=2 };
	stHMM* m_pHmm;
	CBaumWelch* m_pBaum;

	void initialize( void );
	void printModel( stHMM* model );

	void testUsingStateHmm( void );
	void testUsingModelHmm( void );

	void saveResult( void ); // Model-unit HMM‚ÌŒ‹‰Ê‚ğ‹L˜^‚·‚éŠÖ”
	void saveResult(int nVehicleNo, int nIndex, int* pnStateArray); // State-unit HMM‚ÌŒ‹‰Ê‚ğ‹L˜^‚·‚éŠÖ”

	double* m_pdResult;
	int m_nSumCalcTime;
};