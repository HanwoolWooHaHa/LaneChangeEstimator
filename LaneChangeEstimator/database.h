#pragma once

#include "defines.h"

class QFile;

/**
* @class	CDatabase
* @author	Hanwool Woo
* @version	1.10
* @date	Creation date: 2016/01/16 \n
* 			    Last revision date: 2016/01/16 HanwoolWoo
* @brief	this class is for saving measured data
*/
class CDatabase
{
public:
    static CDatabase* GetInstance()
    {
        static CDatabase* instance = new CDatabase();
        return instance;
    }

    ~CDatabase() {}

	/**
	* @fn Initialize
	* @param void
	* @return void
	* @remark this function is to initialize private members
	*/
	void Initialize( void );

	/**
	* @fn LoadData
	* @param int mode
	* @return int
	* @remark this function is to load the data such as the traffic data and the line points
	*/
	int LoadData( int mode, int nType=0 );

	double GetData( int nDataType, int nIndex, int nT, int nColumn = 0 );
	int GetNumData( void );
	int GetDataInfo( int nIndex, int nColumn );

	double GetProbabilityUsingGapAcceptance( int nIndex, int nTime );

	int LoadStateTransitionMatrix( void );
	int SaveStateTransitionMatrix( void );
	double GetStateTransitionMatrix( int i, int j )
	{
		return m_dStateTransitionMatrix[i][j];
	}

	void* GetDataPointer( int mode )
	{
		switch( mode )
		{
		case FEATURE: return reinterpret_cast<void*>(m_dFeatureData);
		}
	}

	void SaveModel( stHMM* model, int mode );
	void LoadModel( stHMM* model, int mode );

private:
	CDatabase();

	int loadLineFiles( void );
	int saveLinePt( int* pnNumPt, double (*pdData)[500][2] );
	int saveApproximateCurve( double (*pdData)[3] );

	/**
	* @fn loadApproximateCurves
	* @param void
	* @return int
	* @remark this function is to load the data of approximate curves
	*/
	int loadApproximateCurves( void );

	/**
	* @fn loadTrafficData
	* @param int nMode : TRAINER, TESTER
	* @return int
	* @remark this function is to load the traffic data for the training and the tests
	*/
	int loadTrafficData( int nMode, int nType );

	/**
	* @fn saveData
	* @param int nMode : TRAINER, TESTER
	* @return int
	* @remark this function is to load the traffic data for the training and the tests
	*/
	int saveData( int nMode );

	QFile* m_pOpenFile;

	// Coefficients of Approximate curves
	double m_dApproximateCurves[NUM_LINE][3];

	int m_nNumTrafficData;
	int m_nDataInfo[NUM_TRAFFIC_DATA][5]; // 0:vehicle no, 1:data length, 2:ground truth
	double m_dTrafficData[NUM_TRAFFIC_DATA][T_MAX][NUM_COLUMN];
	double m_dFeatureData[NUM_TRAFFIC_DATA][T_MAX][FEATURE_VECTOR_DIMENSION];
	int m_nLabelData[NUM_TRAFFIC_DATA][T_MAX];

	double m_dStateTransitionMatrix[NUM_STATE][NUM_STATE];
};