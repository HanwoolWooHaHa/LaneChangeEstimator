#pragma once

class CDatabase;
class CLineExtractor;

/**
* @class	CExtractor
* @author	Hanwool Woo
* @version	1.10
* @date	Creation date: 2016/05/11 \n
* 			    Last revision date: 2016/05/11 HanwoolWoo
* @brief	this class is implemented for the feature extraction
*/
class CExtractor
{
public:
	static CExtractor* GetInstance()
	{
		static CExtractor* instance = new CExtractor();
		return instance;
	}

	~CExtractor();

	void Initialize(void);

	void Extract(int nTick);

private:
	CExtractor();

	/**
	* @fn extractApproximateCurves
	* @param dGlobalOwnX:���E���W�n�ɂ����鎩�Ԃ�X���W, dGlobalOwnY:���E���W�n�ɂ����鎩�Ԃ�Y���W, dAttitude:���E���W�n�ɂ����鎩�Ԃ̎p���p(deg)
	* @return void
	* @remark ���Ԃ̍��E�����̋ߎ��Ȑ������߂�֐��D*/
	void extractApproximateCurves(double dGlobalOwnX, double dGlobalOwnY, double dAttitude);

	void newTransMarksDataMemory(int nNumMarks);
	void deleteTransMarksDataMemory(int nNumMarks);

	/**
	* @fn findPrecedingVehicle
	* @param nTick:����, dGlobalOwnX:���E���W�n�ɂ����鎩�Ԃ�X���W, dGlobalOwnY:���E���W�n�ɂ����鎩�Ԃ�Y���W, nNumAdjVehicles:���ӎԗ��̐�
	* @return int:Preceding vehicle��index
	* @remark ���Ԃ̐�s�Ԃ�T������֐��D*/
	int findPrecedingVehicle(int nTick, double dGlobalOwnX, double dGlobalOwnY, int nNumAdjVehicles);

	/**
	* @fn findLeadVehicle
	* @param nTick:����, nTargetIndex:����ΏێԂ�index, nNumAdjVehicles:���ӎԗ��̐�
	* @return int:Lead vehicle��index
	* @remark Lead vehicle��T���֐��D*/
	int findLeadVehicle(int nTick, int nTargetIndex, int nNumAdjVehicles);

	/**
	* @fn findRearVehicle
	* @param nTick:����, nTargetIndex:����ΏێԂ�index, nNumAdjVehicles:���ӎԗ��̐�
	* @return int:Rear vehicle��index
	* @remark Rear vehicle��T���֐��D*/
	int findRearVehicle(int nTick, int nTargetIndex, int nNumAdjVehicles);

	/**
	* @fn calculateAdjAmounts
	* @param nTick:����, nPrecedingIndex:���Ԃ̐�s�Ԃ�Index, nTargetIndex:����ΏێԂ�index, nLeadIndex:����ΏێԂ̐�s��, nRearIndex:����ΏێԂ̌㑱��, pdAdjAmounts:���Ηʂ��L�^�����z��
	* @return bool true:�v�Z����, false:error
	* @remark ����ΏێԎ���̑��Ηʂ��v�Z����֐��D*/
	bool calculateAdjAmounts(int nTick, int nPrecedingIndex, int nTargetIndex, int nLeadIndex, int nRearIndex, double* pdAdjAmounts);

	/**
	* @fn extractPotentialFeature
	* @param double pdAdjAmounts:���Ηʂ��L�^����Ă���f�[�^�z��
	* @return double:potential feature value
	* @remark ���Ηʂ���potential feature�𒊏o����֐��D*/
	double extractPotentialFeature(double* pdAdjAmounts);

	CDatabase* m_pDatabase;
	CLineExtractor* m_pLineExtractor;

	double** m_ppdLeftTransMarksData;
	double** m_ppdRightTransMarksData;

	double** m_ppdLeftApproximateMarksData;
	double** m_ppdRightApproximateMarksData;
};