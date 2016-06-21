#pragma once

#include "defines.h"

/**
* @class	CSchlechtriemen
* @author	Hanwool Woo
* @version	1.10
* @date	Creation date: 2016/02/20 \n
* 			    Last revision date: 2016/02/20 HanwoolWoo
* @brief	this class is for saving measured data
*/
class CSchlechtriemen
{
public:
	static CSchlechtriemen* GetInstance()
    {
        static CSchlechtriemen* instance = new CSchlechtriemen();
        return instance;
    }

    ~CSchlechtriemen() {}

	/**
	* @fn Extract
	* @param int nNumData, void* pRawData, void* pFeaData, void* pLabData, void* pDataInfo
	* @return int
	* @remark this function is for the feature extraction
	*/
	int Extract( int nMode, int nType, int nNumData, void* pRawData, void* pFeaData, void* pLabData, void* pDataInfo, void* pStateTransition  );


private:
	CSchlechtriemen();
	
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
};