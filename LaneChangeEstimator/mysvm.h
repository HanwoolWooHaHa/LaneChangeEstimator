#pragma once

#include "methodology.h"
#include "Library\libsvm-3.20\svm.h"
#include <list>
#include "defines.h"

typedef struct
{
    double x[FEATURE_VECTOR_DIMENSION * WINDOW_SIZE];
    signed char label;
	double prob;
}stFeatureVector;

/**
* @class	CMySvm
* @author	Hanwool Woo
* @version	1.10
* @date	Creation date: 2016/01/20 \n
* 			    Last revision date: 2016/01/20 HanwoolWoo
* @brief	this class is the implementation of Support Vector Machine using LibSVM
*/
class CMySvm : public CMethodology
{
public:
	CMySvm();
	~CMySvm();

	virtual void Train( int nType );
	virtual void Test( void );

private:
	void setParameter( svm_parameter* param );
	int makePacketForSVM( void );
	int registerVectorToList( int nIndex, int nT );
	bool judge( int& nDataIndex, int& nTime, double dResult, double* dProb );
	void loadModel( svm_model* pModel );
	void saveModel( svm_model* model );
	void saveResult( void );
	double bayesianFilter( int nTime, double dResult, double* dProb );

	std::list<stFeatureVector> m_pt_list;

	svm_model* m_pModel;

	int m_nParamCost;

	double m_dProbabiliy[NUM_STATE][T_MAX];
};