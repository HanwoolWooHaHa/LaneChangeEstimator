#pragma once

#include "defines.h"

/**
* @class	CMandalia
* @author	Hanwool Woo
* @version	1.10
* @date	Creation date: 2016/01/18 \n
* 			    Last revision date: 2016/01/18 HanwoolWoo
* @brief	this class is for saving measured data
*/
class CMandalia
{
public:
	static CMandalia* GetInstance()
    {
        static CMandalia* instance = new CMandalia();
        return instance;
    }

    ~CMandalia() {}

	/**
	* @fn Extract
	* @param int nNumData, void* pRawData, void* pFeaData, void* pLabData, void* pDataInfo
	* @return int
	* @remark this function is for the feature extraction
	*/
	int Extract( int nMode, int nType, int nNumData, void* pRawData, void* pFeaData, void* pLabData, void* pDataInfo, void* pStateTransition  );


private:
	CMandalia();
	
	/**
	* @fn findCenterLine
	* @param int nStartLane, int nEndLane
	* @return int
	* @remark this function is to find the center line
	*/
	int findCenterLine( int nStartLane, int nEndLane );
};