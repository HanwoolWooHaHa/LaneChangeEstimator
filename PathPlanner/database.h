#pragma once

#include "constant.h"
#include <QtCore\qstring.h>

class QFile;

/**
* @class	CDatabase
* @author	Hanwool Woo
* @version	1.10
* @date	Creation date: 2015/12/23 \n
* 			    Last revision date: 2015/12/23 HanwoolWoo
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

    ~CDatabase();

	enum{ MEASUREMENT = 0, FILE, LANE, FEATURE, ESTIMATION_RESULT, SIMULATION, NORTHEAST, SOUTHWEST };

	/**
	* @fn LoadMeasurementData
	* @param fileName : measurement file path
	* @return -2 : T_MAX overed, -1 : Failed to open the file, 1 : Done
	* @remark this function is to load a measurement data and save the data to private members
	*/
	int LoadMeasurementData( QString fileName );

	int LoadLaneData( int mode );

	int LoadStateTransitionMatrix(void);

	/**
	* @fn Initialize
	* @param void
	* @return void
	* @remark this function is to initialize private members
	*/
	void Initialize( void );

	int GetDataLength( void ) { return m_nDataLength; }

	int GetNumChangingVehicles( void ) { return m_nNumChangingVehicles; }
	int GetNumKeepingVehicles( void ) { return m_nNumKeepingVehicles; }
	int GetNumAdjacentVehicles(void) { return m_nNumAdjacentVehicles; }

	int GetNumTargetVehicles(void) { return m_nNumTargetVehicles; }
	void SetNumTargetVehicles(int nNumber) { m_nNumTargetVehicles = nNumber; }

	void GenerateData(void);

	double GetData( int type, int time, int index, int column = 0 )
	{
		switch( type )
		{
		case MEASUREMENT:
			return m_dMeasureData[time][index];

		case LANE:
			return m_dLaneData[time][index][column];

		case FILE:
			return m_dFileData[time][index];

		case SIMULATION:
			return m_dEstimationResultData[index][time][column];

		default:
			return -1;
		}
	}

	void SetData(int nType, int nTime, int nIndex, double dValue, int nColumn = 0)
	{
		switch (nType)
		{
		case MEASUREMENT:
			m_dMeasureData[nTime][nIndex] = dValue;
			break;

		case SIMULATION:
			m_dEstimationResultData[nIndex][nTime][nColumn] = dValue;
			break;
		}
	}

	int SaveData( int nType, int nLength = 0, int nColumn = 0 );

	void SetOrigin( void );

	int GetNumMarks( void )
	{
		return MARKS_MAX;
	}
	int GetCurrentLane(void)
	{
		return m_nCurrentLane;
	}

	void SetNumNearMarks( int number )
	{
		m_nNumNearMarks = number;
	}

	int GetNumNearMarks( void )
	{
		return m_nNumNearMarks;
	}

	void SetNearMarks( int i, int index, double x, double y )
	{
		m_dNearMarks[i][0] = index;
		m_dNearMarks[i][1] = x;
		m_dNearMarks[i][2] = y;
	}

	double GetNearMarks( int index, int column )
	{
		return m_dNearMarks[index][column];
	}

	void SaveNearMarks( void );

	void SetRecordData( int nTick, int nCol, double dVal )
	{
		m_dRecordData[nTick][nCol] = dVal;
	}

	void SaveRecordFile( void );

	double GetFeatureData(int nNoAdjVehicle, int nTick, int nDimension)
	{
		return m_dFeatureData[nNoAdjVehicle][nTick][nDimension];
	}
	void SetFeatureData(int nNoAdjVehicle, int nTick, int nDimension, double dValue)
	{
		m_dFeatureData[nNoAdjVehicle][nTick][nDimension] = dValue;
	}

	bool GetTargetOnOff(int nIndex)
	{
		return m_bTargetOnOff[nIndex];
	}
	void SetTargetOnOff(int nIndex, bool bFlag)
	{
		m_bTargetOnOff[nIndex] = bFlag;
	}

	double GetEstimationResultData(int nTick, int nAdjVehicleIndex, int nPacket)
	{
		return m_dEstimationResultData[nAdjVehicleIndex][nTick][nPacket];
	}
	void SetEstimationResultData(int nTick, int nAdjVehicleIndex, int nPacket, double dValue)
	{
		m_dEstimationResultData[nAdjVehicleIndex][nTick][nPacket] = dValue;
	}

	double GetStateTransitionMatrix(int i, int j)
	{
		return m_dStateTransitionMatrix[i][j];
	}
	void FilteringByKalman(int nTick);

private:
	CDatabase();
	void convertGpsToCoordinateInMeasurement( void );
	void convertGpsToCoordinateInLine( void );
	int saveLaneData( void );
	int saveFeatureData(void);
	int saveEstimationResultData(void);
	int findCurrentLane(void);

	QFile* m_pOpenFile;

	unsigned int MARKS_MAX;
	int m_nNumChangingVehicles;
	int m_nNumKeepingVehicles;
	int m_nNumAdjacentVehicles;
	int m_nNumTargetVehicles;

	int m_nDataLength;
	int m_nDataSize;
	double m_dFileData[T_MAX][FILE_COLUMN_MAX];
	double m_dMeasureData[T_MAX][MDATA_COLUMN_MAX];
	int m_nNumNearMarks;
	double m_dNearMarks[1000][3];

	int m_nCurrentLane;
	double m_dLaneData[NUM_LANE][LINE_MAX_INDEX][2];

	double m_dRecordData[T_MAX][RECORD_COLUMN];

	double m_dEstimationResultData[ADJ_VEH_MAX][T_MAX][RESULT_PACKET_NUM];
	double m_dFeatureData[ADJ_VEH_MAX][T_MAX][FEATURE_VECTOR_DIMENSION];
	bool m_bTargetOnOff[ADJ_VEH_MAX];

	double m_dStateTransitionMatrix[NUM_STATE][NUM_STATE];
};