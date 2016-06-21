#pragma once

class CMethodology;

/**
* @class	CEstimator
* @author	Hanwool Woo
* @version	1.10
* @date	Creation date: 2016/01/18 \n
* 			    Last revision date: 2016/01/18 HanwoolWoo
* @brief	this class is for saving measured data
*/
class CEstimator
{
public:
	CEstimator( int nMethod );
	~CEstimator();

	void Train( int nType );

	void Test( void );

private:
	CMethodology* m_pMethod;
};