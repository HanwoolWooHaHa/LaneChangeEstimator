#pragma once

/**
* @class	CFilter
* @author	Hanwool Woo
* @version	1.10
* @date	Creation date: 2016/01/18 \n
* 			    Last revision date: 2016/01/18 HanwoolWoo
* @brief	this class is for saving measured data
*/
class CFilter
{
public:
	static CFilter* GetInstance()
    {
        static CFilter* instance = new CFilter();
        return instance;
    }

    ~CFilter();

	int MovingAverageFilter( int nData, int nNumData, void* pDataInfo, void* pData, int nColumn=0 );
	int MedianFilter( int nNumVehicles, void* pDataInfo, void* pData, int nColumn );
	
private:
	CFilter();

	int movingAverageFilterForTrafficData( int nNumData, void* pDataInfo, void* pData );
	int movingAverageFilterForFeatureData( int nNumData, void* pDataInfo, void* pData, int nColumn );

	void sort( int nNum, double* pdData );
};