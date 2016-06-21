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
	* @param dGlobalOwnX:世界座標系における自車のX座標, dGlobalOwnY:世界座標系における自車のY座標, dAttitude:世界座標系における自車の姿勢角(deg)
	* @return void
	* @remark 自車の左右白線の近似曲線を求める関数．*/
	void extractApproximateCurves(double dGlobalOwnX, double dGlobalOwnY, double dAttitude);

	void newTransMarksDataMemory(int nNumMarks);
	void deleteTransMarksDataMemory(int nNumMarks);

	/**
	* @fn findPrecedingVehicle
	* @param nTick:時間, dGlobalOwnX:世界座標系における自車のX座標, dGlobalOwnY:世界座標系における自車のY座標, nNumAdjVehicles:周辺車両の数
	* @return int:Preceding vehicleのindex
	* @remark 自車の先行車を探索する関数．*/
	int findPrecedingVehicle(int nTick, double dGlobalOwnX, double dGlobalOwnY, int nNumAdjVehicles);

	/**
	* @fn findLeadVehicle
	* @param nTick:時間, nTargetIndex:推定対象車のindex, nNumAdjVehicles:周辺車両の数
	* @return int:Lead vehicleのindex
	* @remark Lead vehicleを探す関数．*/
	int findLeadVehicle(int nTick, int nTargetIndex, int nNumAdjVehicles);

	/**
	* @fn findRearVehicle
	* @param nTick:時間, nTargetIndex:推定対象車のindex, nNumAdjVehicles:周辺車両の数
	* @return int:Rear vehicleのindex
	* @remark Rear vehicleを探す関数．*/
	int findRearVehicle(int nTick, int nTargetIndex, int nNumAdjVehicles);

	/**
	* @fn calculateAdjAmounts
	* @param nTick:時間, nPrecedingIndex:自車の先行車のIndex, nTargetIndex:推定対象車のindex, nLeadIndex:推定対象車の先行車, nRearIndex:推定対象車の後続車, pdAdjAmounts:相対量が記録される配列
	* @return bool true:計算完了, false:error
	* @remark 推定対象車周りの相対量を計算する関数．*/
	bool calculateAdjAmounts(int nTick, int nPrecedingIndex, int nTargetIndex, int nLeadIndex, int nRearIndex, double* pdAdjAmounts);

	/**
	* @fn extractPotentialFeature
	* @param double pdAdjAmounts:相対量が記録されているデータ配列
	* @return double:potential feature value
	* @remark 相対量からpotential featureを抽出する関数．*/
	double extractPotentialFeature(double* pdAdjAmounts);

	CDatabase* m_pDatabase;
	CLineExtractor* m_pLineExtractor;

	double** m_ppdLeftTransMarksData;
	double** m_ppdRightTransMarksData;

	double** m_ppdLeftApproximateMarksData;
	double** m_ppdRightApproximateMarksData;
};