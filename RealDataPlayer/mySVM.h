#pragma once

#include "method.h"
#include "Library\LibSVM\svm.h"
#include <list>
#include "constant.h"

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
* @date	Creation date: 2016/05/12 \n
* 			    Last revision date: 2016/05/12 HanwoolWoo
* @brief	this class is the implementation of Support Vector Machine using LibSVM
*/
class CMySvm : public CMethod
{
public:
	CMySvm();
	~CMySvm();

	virtual void Train(int nType);
	virtual bool Test(int nTick, int nTargetIndex);

private:
	void setParameter(svm_parameter* param);
	
	bool judge(int& nDataIndex, int& nTime, double dResult, double* dProb);
	void loadModel(svm_model* pModel);
	void saveModel(svm_model* model);
	void saveResult(void);
	double bayesianFilter(int nTime, double dResult, double* dProb);

	/**
	* @fn makePacketForSVM
	* @param int nTick:���݂̎���, int nTargetIndex:����ΏێԂ�index
	* @return void
	* @remark SVM��p���Đ�����s�����߁C�f�[�^����packet�𐶐�����֐��D*/
	void makePacketForSVM(int nTick, int nTargetIndex);

	/**
	* @remark �����x�N�g�����o�^����郊�X�g*/
	std::list<stFeatureVector> m_feature_vector_list;

	svm_model* m_pModel;

	int m_nParamCost;

	double m_dProbabiliy[NUM_CLASS][T_MAX];
};