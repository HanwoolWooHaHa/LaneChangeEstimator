/**
* @file	database.cpp
* @version	1.00
* @author	Hanwool Woo
* @date	Creation date: 2015/12/23
* @brief	this file is for saving measured data
*/

#include "database.h"
#include "sensor.h"
#include "filter.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qdebug.h>
#include <qmath.h>
#include <QTime>


#define LANE_NORTHEAST "Lane/B3_2E.csv"
#define LANE_SOUTHWEST "Lane/B3_2W.csv"
/*********************************************************************/
CDatabase::CDatabase()
{
	Initialize();
}

CDatabase::~CDatabase()
{
}
/*********************************************************************/
void CDatabase::Initialize( void )
{
	m_nNumChangingVehicles = m_nNumKeepingVehicles = m_nNumAdjacentVehicles = 0;

	memset( m_dFileData, 0, sizeof( double ) * T_MAX * FILE_COLUMN_MAX );
	memset( m_dMeasureData, 0, sizeof( double ) * T_MAX * MDATA_COLUMN_MAX );
	memset( m_dNearMarks, 0, sizeof( double ) * 1000 * 3 );
	memset( m_dRecordData, 0, sizeof( double ) * T_MAX * RECORD_COLUMN );
	memset(m_dLaneData, 0, sizeof(double)* NUM_LANE * LINE_MAX_INDEX * 2);
	memset(m_dFeatureData, 0, sizeof(double)*ADJ_VEH_MAX*T_MAX*FEATURE_VECTOR_DIMENSION);
	memset(m_bTargetOnOff, 0, sizeof(bool)*ADJ_VEH_MAX);
	memset(m_dEstimationResultData, 0, sizeof(double)* ADJ_VEH_MAX * T_MAX * RESULT_PACKET_NUM);
	memset(m_dStateTransitionMatrix, 0, sizeof(double)* NUM_STATE * NUM_STATE);
}

void CDatabase::GenerateData(void)
{
	qsrand((uint)QTime::currentTime().msec());
	double dRand = 0.0;

	m_nNumAdjacentVehicles = NUM_ADJ_VEHICLES;

	m_dEstimationResultData[0][0][PACKET_SIMUL_POSX] = 0.0;
	m_dEstimationResultData[0][0][PACKET_SIMUL_POSY] = LANE_WIDTH * 1.5;
	m_dEstimationResultData[0][0][PACKET_SIMUL_VELX] = 16.67; // unit : m/s = 60 km/h
	m_dEstimationResultData[0][0][PACKET_SIMUL_VELY] = 0.0;
	
	for (int n = 1; n < m_nNumAdjacentVehicles; n++)
	{
		dRand = qrand();
		
		int nLane = (int)dRand % 3;

		double dVelocity = (int)dRand % 5;
		dVelocity += 60.0;
		
		dVelocity = dVelocity * 0.277777777; // unit : m/s

		dRand = (int)dRand % 628;
		dRand /= 100;
		dRand = 100.0 * qSin(dRand); // -100~100

		double dPosX = dRand;
		double dPosY = nLane * LANE_WIDTH + 0.5 * LANE_WIDTH;

		m_dEstimationResultData[n][0][PACKET_SIMUL_POSX] = dPosX;
		m_dEstimationResultData[n][0][PACKET_SIMUL_POSY] = dPosY;
		m_dEstimationResultData[n][0][PACKET_SIMUL_VELX] = dVelocity;
		m_dEstimationResultData[n][0][PACKET_SIMUL_VELY] = 0.0;

		bool bFlag = true;
		for (int k = 0; k < n; k++)
		{
			if (dPosY == m_dEstimationResultData[k][0][PACKET_SIMUL_POSY])
			{
				if (qAbs(dPosX - m_dEstimationResultData[k][0][PACKET_SIMUL_POSX]) < SAFETY_RANGE)
				{
					bFlag = false;
					break;
				}
			}
		}
		if (qAbs(dPosX) < SAFETY_RANGE)
			bFlag = false;

		if (!bFlag)
		{
			n--;
			continue;
		}

		qDebug() << "CDatabase.cpp @ No." << n << ":  Lane = " << nLane << ",  Vel = " << dVelocity << ",  DelX = " << dRand;
	}
}

int CDatabase::LoadMeasurementData( QString fileName )
{
	m_pOpenFile = new QFile( fileName );

	if( !m_pOpenFile->open(QIODevice::ReadOnly) )
    {
        delete m_pOpenFile;
		return -1;
    }

	QTextStream in( m_pOpenFile );
	std::string std;

	int nLaneChangingVehicles = 0;
	int nLaneKeepingVehicles = 0;

	QString splitline = in.readLine();

#if defined(SELECTED_VEHICLE)
	{
		QString line = in.readLine();
		QStringList list = line.split(',');
		int nStep = 0;

		int j = 0;
		for( ; j<list.size(); j++ )
		{
			if( nStep == 0 )
			{
				std = list.at(j).toStdString();
				if( std == "" ) nStep = 1;
			}
			else if( nStep == 1 ) // Lane changing vehicles
			{
				std = list.at(j).toStdString();
				nLaneChangingVehicles++;
				if( std == "" ) nStep = 2;
			}
			else // Lane keeping vehicles
			{
				std = list.at(j).toStdString();
				nLaneKeepingVehicles++;
				if( std == "" ) nStep = 3;
			}
		}

		m_nNumChangingVehicles = nLaneChangingVehicles / 2;
		m_nNumKeepingVehicles = nLaneKeepingVehicles / 2;
		m_nDataSize = j;
	}
#elif defined(ALL_VEHICLE)
	{
		QString line = in.readLine();
		QStringList list = line.split(',');

		m_nNumAdjacentVehicles = (list.size() - 4) / 2;
	}
#endif

	qDebug() << endl;
	qDebug() << "database.cpp @ " << "Changing vehicles : " << m_nNumChangingVehicles << ", Keeping vehicles : " << m_nNumKeepingVehicles;

	int index = 0;
	while( !in.atEnd() )
    {
        QString line = in.readLine();
        QStringList list = line.split(',');

        for( int j=0; j<list.size(); j++ )
        {
            m_dFileData[index][j] = list.at(j).toDouble();
        }

        index++;

        if( index >= T_MAX ) return -2;
    }
	m_nDataLength = index;

	qDebug() << "database.cpp @ " << "Data length : " << m_nDataLength << ", Data size : " << m_nDataSize;
	qDebug() << "database.cpp @ " << "Data was successfully loaded";

	int nMode;

	if( m_dFileData[3][PACKET_OWN_GLOBAL_ATT] < 180.0 )
	{
		MARKS_MAX = 26475;
		nMode = NORTHEAST;
	}
	else
	{
		MARKS_MAX = 25568;
		nMode = SOUTHWEST;
	}

	//!  éûä‘ç∑ÇåvéZÇ∑ÇÈ
	double dTemp[T_MAX] = { 0.0 };
	for (int t = 1; t < m_nDataLength; t++)
	{
		double dt = m_dFileData[t][PACK_TIME] - m_dFileData[t-1][PACK_TIME];

		if (dt > 1.0)
			dt = dt - (int)dt;

		dTemp[t] = dt;
	}
	m_dFileData[0][PACK_TIME] = 0.0;
	for (int t = 0; t < m_nDataLength; t++)
	{
		m_dFileData[t][PACK_TIME] = dTemp[t];
	}

	//CFilter::GetInstance()->MovingAverageFilter(m_nDataLength, m_nNumAdjacentVehicles, reinterpret_cast<void*>(m_dFileData));

	//! é‘ê¸èÓïÒÇì«Ç›çûÇﬁ
	LoadLaneData(nMode);

	//! Globalç¿ïWånÇ÷à íuÇïœä∑Ç∑ÇÈ
	convertGpsToCoordinateInMeasurement();

	//! Bayesian FilterÇ…égÇÌÇÍÇÈèÛë‘ëJà⁄ämó¶Çì«Ç›çûÇﬁ
	LoadStateTransitionMatrix();

	return 0;
}

void CDatabase::FilteringByKalman(int nTick)
{
	CFilter::GetInstance()->KalmanFilter(nTick, m_nNumAdjacentVehicles, reinterpret_cast<void*>(m_dFeatureData), reinterpret_cast<void*>(m_dFileData));
}

int CDatabase::LoadStateTransitionMatrix(void)
{
	QString file = "Model/stateTransitionMatrix.csv";

	m_pOpenFile = new QFile(file);

	if (!m_pOpenFile->open(QIODevice::ReadOnly))
	{
		delete m_pOpenFile;
		return -1;
	}

	QTextStream in(m_pOpenFile);
	int nState = 0;

	while (!in.atEnd())
	{
		QString line = in.readLine();
		QStringList list = line.split(',');

		for (int j = 0; j<list.size(); j++)
		{
			m_dStateTransitionMatrix[nState][j] = list.at(j).toDouble();
		}

		nState++;
	}

	delete m_pOpenFile;

	return 0;
}

int CDatabase::LoadLaneData( int mode )
{
	if (mode == NORTHEAST)
		m_pOpenFile = new QFile(LANE_NORTHEAST);
	else if (mode == SOUTHWEST)
		m_pOpenFile = new QFile(LANE_SOUTHWEST);

	if (!m_pOpenFile->open(QIODevice::ReadOnly))
	{
		delete m_pOpenFile;
		return -1;
	}

	QTextStream in(m_pOpenFile);
	int index = 0;

	while (!in.atEnd())
	{
		QString line = in.readLine();
		QStringList list = line.split(',');

		for (int j = 0; j<list.size(); j++)
		{
			switch (j)
			{
			case 0: m_dLaneData[0][index][0] = list.at(j).toDouble(); break;
			case 1: m_dLaneData[0][index][1] = list.at(j).toDouble(); break;
			case 2: m_dLaneData[1][index][0] = list.at(j).toDouble(); break;
			case 3: m_dLaneData[1][index][1] = list.at(j).toDouble(); break;
			case 4: m_dLaneData[2][index][0] = list.at(j).toDouble(); break;
			case 5: m_dLaneData[2][index][1] = list.at(j).toDouble(); break;
			case 6: m_dLaneData[3][index][0] = list.at(j).toDouble(); break;
			case 7: m_dLaneData[3][index][1] = list.at(j).toDouble(); break;
			}
		}

		index++;
		if (index > MARKS_MAX)
			break;;
	}

	convertGpsToCoordinateInLine();

	//saveLaneData();
	//sortLanePoint();

	//m_nCurrentLane = findCurrentLane();

	qDebug() << "database.cpp @ Line Marking Data was Loaded!!";

	return 1;
}

int CDatabase::SaveData( int nType, int nLength, int nColumn )
{
	int nRet = 0;

	switch( nType )
	{
	case MEASUREMENT:
		break;

	case LANE:
		nRet = saveLaneData();
		break;

	case FEATURE:
		nRet = saveFeatureData();
		break;

	case ESTIMATION_RESULT:
		nRet = saveEstimationResultData();
		break;
	}

	return nRet;
}

void CDatabase::SetOrigin( void )
{
	CSensor::GetInstance()->SetOrigin( 139.7549931, 35.60989152 );
}

void CDatabase::SaveNearMarks( void )
{
	QString fileName = "Lane/NearMarks.csv";

	QFile* fp = new QFile( fileName );
    if( !fp->open(QIODevice::WriteOnly) )
    {
        return;
    }

	QTextStream* out = new QTextStream( fp );

	*out << "index" << "," << "x" << "," << "y" << endl;

	for( int t=0; t<m_nNumNearMarks; t++ )
	{
		*out << m_dNearMarks[t][0] << ","  << m_dNearMarks[t][1] << ","  << m_dNearMarks[t][2] <<endl;
	}

	fp->close();

    delete fp;
    delete out;
}

void CDatabase::SaveRecordFile( void )
{
	QString fileName = "Log/record.csv";

	QFile* fp = new QFile( fileName );
    if( !fp->open(QIODevice::WriteOnly) )
    {
        return;
    }

	QTextStream* out = new QTextStream( fp );

	//*out << "time" << "," << "x" << "," << "y" << endl;

	for( int t=0; t<m_nDataLength; t++ )
	{
		*out << m_dRecordData[t][0] << ","  << m_dRecordData[t][1] << ","  << m_dRecordData[t][2] <<endl;
	}

	fp->close();

    delete fp;
    delete out;
}

int CDatabase::saveLaneData( void )
{
	QString fileName = "Lane/CoordinateLaneData.csv";

	QFile* fp = new QFile(fileName);
	if (!fp->open(QIODevice::WriteOnly))
	{
		return -1;
	}

	QTextStream* out = new QTextStream(fp);

	*out << "Longitude1" << "," << "Latitude1" << "," << "Longitude2" << "," << "Latitude2" << "," << "Longitude3" << "," << "Latitude3" << "," << "Longitude4" << "," << "Latitude4" << endl;

	for (int t = 0; t<MARKS_MAX; t++)
	{
		for (int n = 0; n<NUM_LANE; n++)
		{
			*out << m_dLaneData[n][t][0] << "," << m_dLaneData[n][t][1] << ",";
		}
		*out << endl;
	}

	fp->close();

	delete fp;
	delete out;

	return 1;
}

int CDatabase::saveFeatureData(void)
{
	QString fileName = "../../Log/Feature/RealDataPlayer/feature.csv";

	QFile* fp = new QFile(fileName);
	if (!fp->open(QIODevice::WriteOnly))
	{
		return -1;
	}

	QTextStream* out = new QTextStream(fp);

	//! é¸ï”é‘óºêî
	int nNumAdjVehicles = GetNumAdjacentVehicles();

	for (int t = 0; t<m_nDataLength; t++)
	{
		*out << t << ",";

		for (int n = 0; n<nNumAdjVehicles; n++)
		{
			for (int k = 0; k < FEATURE_VECTOR_DIMENSION; k++)
			{
				*out << m_dFeatureData[n][t][k] << ",";
			}
		}

		*out << endl;
	}

	fp->close();

	delete fp;
	delete out;

	qDebug() << "database.cpp @ Saving the feature data was completed";

	return 1;
}

int CDatabase::saveEstimationResultData(void)
{
	QString fileName = "../../Log/Result/RealDataPlayer/estimation_result.csv";

	QFile* fp = new QFile(fileName);
	if (!fp->open(QIODevice::WriteOnly))
	{
		return -1;
	}

	QTextStream* out = new QTextStream(fp);

	//! é¸ï”é‘óºêî
	int nNumAdjVehicles = GetNumAdjacentVehicles();

	for (int t = 0; t<m_nDataLength; t++)
	{
		*out << m_dFileData[t][PACK_TIME] << ",";

		for (int n = 0; n<nNumAdjVehicles; n++)
		{
			*out << n << ",";
			for (int k = 0; k < FEATURE_VECTOR_DIMENSION; k++)
			{
				*out << m_dFeatureData[n][t][k] << ",";
			}

			//! êÑíËåãâ ÇãLò^Ç∑ÇÈ
			*out << m_dEstimationResultData[n][t][RESULT_PACKET_CLASS] << ",";
		}

		*out << endl;
	}

	fp->close();

	delete fp;
	delete out;

	qDebug() << "database.cpp @ Saving the estimation result data was completed";

	return 1;
}

void CDatabase::convertGpsToCoordinateInMeasurement( void )
{
	//! é©é‘ÇÃà íuÇïœä∑Ç∑ÇÈ
	for( int t=0; t<m_nDataLength; t++ )
	{
		double dPosX = 0.0;
		double dPosY = 0.0;

		CSensor::GetInstance()->CalcGlobalPosition( m_dFileData[t][PACKET_OWN_GLOBAL_X], m_dFileData[t][PACKET_OWN_GLOBAL_Y], &dPosX, &dPosY );

		//! Globalç¿ïWånÇ…Ç®ÇØÇÈà íuÇãLò^Ç∑ÇÈ
		m_dMeasureData[t][PACKET_OWN_GLOBAL_X] = dPosX;
		m_dMeasureData[t][PACKET_OWN_GLOBAL_Y] = dPosY;

		//! Globalç¿ïWånÇ…Ç®ÇØÇÈépê®äpÇãLò^Ç∑ÇÈ
		if (t == 0)
		{
			m_dMeasureData[t][PACKET_OWN_GLOBAL_ATT] = m_dFileData[t][PACKET_OWN_GLOBAL_ATT];
			m_nCurrentLane = findCurrentLane();
		}
		else
			m_dMeasureData[t][PACKET_OWN_GLOBAL_ATT] = CSensor::GetInstance()->CalculateAttitude(m_dMeasureData[t - 1][PACKET_OWN_GLOBAL_X], m_dMeasureData[t - 1][PACKET_OWN_GLOBAL_Y], m_dMeasureData[t][PACKET_OWN_GLOBAL_X], m_dMeasureData[t][PACKET_OWN_GLOBAL_Y], m_nCurrentLane, MARKS_MAX, reinterpret_cast<void*>(m_dLaneData));
	}

	

#if defined(SELECTER_VEHICLES)
	//! ëºé‘ÇÃà íuÇïœä∑Ç∑ÇÈ
	for( int t=0; t<m_nDataLength; t++ )
	{
		for( int n=0; n<m_nNumChangingVehicles; n++ )
		{
			int nColumn = n * 2 + 5;
			double dPosX = m_dFileData[t][nColumn]; //0.0;
			double dPosY = m_dFileData[t][nColumn+1]; //0.0;
			double dAzimuth = 0.0;

			//CSensor::GetInstance()->CalcGlobalPosition( 0, m_dFileData[t][nColumn], m_dFileData[t][nColumn+1], &dPosX, &dPosY, &dAzimuth );
			CSensor::GetInstance()->CalcGlobalPosition( m_dMeasureData[t][PACKET_OWN_GLOBAL_X], m_dMeasureData[t][PACKET_OWN_GLOBAL_Y], m_dMeasureData[t][PACKET_OWN_GLOBAL_ATT], &dPosX, &dPosY );
			
			nColumn = n * 3 + 5;
			m_dMeasureData[t][nColumn] = dPosX;
			m_dMeasureData[t][nColumn+1] = dPosY;
			m_dMeasureData[t][nColumn+2] = dAzimuth;
		}

		for( int n=0; n<m_nNumKeepingVehicles; n++ )
		{
			int nColumn = n * 2 + m_nNumChangingVehicles * 2 + 6;
			double dPosX = m_dFileData[t][nColumn]; //0.0;
			double dPosY = m_dFileData[t][nColumn+1]; //0.0;
			double dAzimuth = 0.0;

			//CSensor::GetInstance()->CalcGlobalPosition( 0, m_dFileData[t][nColumn], m_dFileData[t][nColumn+1], &dPosX, &dPosY, &dAzimuth );
			CSensor::GetInstance()->CalcGlobalPosition( m_dMeasureData[t][PACKET_OWN_GLOBAL_X], m_dMeasureData[t][PACKET_OWN_GLOBAL_Y], m_dMeasureData[t][PACKET_OWN_GLOBAL_ATT], &dPosX, &dPosY );
			
			nColumn = n * 3 + m_nNumChangingVehicles * 3 + 6;
			m_dMeasureData[t][nColumn] = dPosX;
			m_dMeasureData[t][nColumn+1] = dPosY;
			m_dMeasureData[t][nColumn+2] = dAzimuth;
		}
	}
#elif defined(ALL_VEHICLE)
	for (int t = 0; t < m_nDataLength; t++)
	{
		for (int n = 0; n < m_nNumAdjacentVehicles; n++)
		{
			int nColumn = n * 2 + 4;
			double dPosX = m_dFileData[t][nColumn]; //0.0;
			double dPosY = m_dFileData[t][nColumn + 1]; //0.0;
			double dAzimuth = 0.0;

			if ( isnan(dPosX) && isnan(dPosY) )
			{
				dPosX = dPosY = dAzimuth = NAN;
			}
			else
			{
				CSensor::GetInstance()->CalcGlobalPosition(m_dMeasureData[t][PACKET_OWN_GLOBAL_X], m_dMeasureData[t][PACKET_OWN_GLOBAL_Y], m_dMeasureData[t][PACKET_OWN_GLOBAL_ATT], &dPosX, &dPosY);
			}

			nColumn = n * 3 + 5;
			m_dMeasureData[t][nColumn] = dPosX;
			m_dMeasureData[t][nColumn + 1] = dPosY;
			m_dMeasureData[t][nColumn + 2] = dAzimuth;
		}
	}
#endif
}

void CDatabase::convertGpsToCoordinateInLine( void )
{
	for (int n = 0; n<NUM_LANE; n++)
	{
		for (int i = 0; i<MARKS_MAX; i++)
		{
			double dPosX = 0.0;
			double dPosY = 0.0;

			CSensor::GetInstance()->CalcGlobalPosition(m_dLaneData[n][i][0], m_dLaneData[n][i][1], &dPosX, &dPosY);

			m_dLaneData[n][i][0] = dPosX;
			m_dLaneData[n][i][1] = dPosY;
		}
	}
}

int CDatabase::findCurrentLane(void)
{
	// é©é‘ÇÃèoî≠à íu
	double dOwnPosX = GetData(MEASUREMENT, 0, PACKET_OWN_GLOBAL_X);
	double dOwnPosY = GetData(MEASUREMENT, 0, PACKET_OWN_GLOBAL_Y);

	// é‘ê¸ì_åQÇÃêî
	int nNumMarks = GetNumMarks();

	double dMin[NUM_LANE];

	for (int n = 0; n<NUM_LANE; n++)
	{
		dMin[n] = DBL_MAX;

		for (int i = 0; i<nNumMarks; i++)
		{
			double dLinePosX = GetData(LANE, n, i, 0);
			double dLinePosY = GetData(LANE, n, i, 1);

			double dDistance = qSqrt((dOwnPosX - dLinePosX)*(dOwnPosX - dLinePosX) + (dOwnPosY - dLinePosY)*(dOwnPosY - dLinePosY));

			if (dDistance < dMin[n])
			{
				dMin[n] = dDistance;
			}
		}
	}

	if (dMin[0] < dMin[2] && dMin[0] < dMin[3] && dMin[1] < dMin[2] && dMin[1] < dMin[3])
		return 0;
	else if (dMin[1] < dMin[0] && dMin[1] < dMin[3] && dMin[2] < dMin[0] && dMin[2] < dMin[3])
		return 1;
	else if (dMin[2] < dMin[0] && dMin[2] < dMin[1] && dMin[3] < dMin[0] && dMin[3] < dMin[1])
		return 2;
	else
		return -1;
}