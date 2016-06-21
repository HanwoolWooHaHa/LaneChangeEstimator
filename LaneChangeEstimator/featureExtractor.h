#pragma once

#include "defines.h"

/**
* @class	CFeatureExtractor
* @author	Hanwool Woo
* @version	1.10
* @date	Creation date: 2016/01/18 \n
* 			    Last revision date: 2016/01/18 HanwoolWoo
* @brief	this class is for saving measured data
*/
class CFeatureExtractor
{
public:
	static CFeatureExtractor* GetInstance()
    {
        static CFeatureExtractor* instance = new CFeatureExtractor();
        return instance;
    }

    ~CFeatureExtractor() {}

	/**
	* @fn Extract
	* @param int nNumData, void* pRawData, void* pFeaData, void* pLabData, void* pDataInfo
	* @return int
	* @remark this function is for the feature extraction
	*/
	int Extract( int nMode, int nType, int nNumData, void* pRawData, void* pFeaData, void* pLabData, void* pDataInfo, void* pStateTransition  );

	/**
	* @fn GetProbabilityUsingGapAcceptance
	* @param int nIndex, int nTime
	* @return double
	* @remark this function is for the feature extraction
	*/
	double GetProbabilityUsingGapAcceptance( int nIndex, int nTime );

private:
	CFeatureExtractor();
	
	/**
	* @fn findCenterLine
	* @param int nStartLane, int nEndLane
	* @return int
	* @remark this function is to find the center line
	*/
	int findCenterLine( int nStartLane, int nEndLane );

	int makeAdjacentVehicleList( int nMode, int nDataIndex, void* pRawData, void* pDataInfo );

	int calculateVehicleGap( int nTime, int nMode, int nDataIndex, void* pRawData, void* pDataInfo, double* dGap );

	void initializeAdjData( void );

	int m_nNumAdjacentVehicles;
	int m_nAdjDataInfo[MAX_ADJACENT][10]; // 0:No 1:data length 2:vehicle length 3:vehicle width
	double m_dAdjacentVehiclesData[MAX_ADJACENT][T_MAX][NUM_ADJACENT_COLUMN]; // 0 : vehivle no.  1 : scene  2 : pos_x  3 : pos_y  4 : Lane 5 : Velocity

	static double m_dProbability[NUM_TRAFFIC_DATA][T_MAX];
};