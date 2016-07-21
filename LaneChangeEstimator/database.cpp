/**
* @file	database.cpp
* @version	1.00
* @author	Hanwool Woo
* @date	Creation date: 2016/01/16
* @brief	this file is for the management of data files
*/
#include "database.h"
#include "lineExtractor.h"
#include "filter.h"
#include "featureExtractor.h"
#include "OtherMethods\mandalia.h"
#include "OtherMethods\schlechtriemen.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qdebug.h>

CDatabase::CDatabase()
{
	Initialize();
}
///////////////////////////////////////////////////////////////////////////
/* Public functions */
///////////////////////////////////////////////////////////////////////////
void CDatabase::Initialize( void )
{
	m_nNumTrafficData = 0;

	memset( m_dApproximateCurves, 0, sizeof( double ) * NUM_LINE * 3 );
	memset( m_dTrafficData, 0, sizeof( double ) * NUM_TRAFFIC_DATA * T_MAX * NUM_COLUMN );
	memset( m_dFeatureData, 0, sizeof( double ) * NUM_TRAFFIC_DATA * T_MAX * FEATURE_VECTOR_DIMENSION );
	memset( m_nLabelData, 0, sizeof( int ) * NUM_TRAFFIC_DATA * T_MAX );
	memset( m_dStateTransitionMatrix, 0, sizeof( double ) * NUM_STATE * NUM_STATE );
}

int CDatabase::LoadData( int nMode, int nType )
{
	int nReturn = 0;

	switch( nMode )
	{
	case LINE_EXTRACTOR:
		nReturn = loadLineFiles();
		break;

	case APPROXIMATE_CURVES:
		nReturn = loadApproximateCurves();
		break;

	case TRAINER:
		nReturn = loadTrafficData( nMode, nType );
		break;

	case TESTER:
		//LoadStateTransitionMatrix();
		nReturn = loadTrafficData( nMode, nType );
		break;

	default:
		return FAIL;
	}

	return nReturn;
}

double CDatabase::GetData( int nDataType, int nIndex, int nT, int nColumn )
{
	switch( nDataType )
	{
	case TRAFFIC:
		return m_dTrafficData[nIndex][nT][nColumn];

	case FEATURE:
		return m_dFeatureData[nIndex][nT][nColumn];

	case LABEL:
		return (double)(m_nLabelData[nIndex][nT]);

	default:
		return FAIL;
	}
}

int CDatabase::GetNumData( void )
{
	return m_nNumTrafficData;
}

int CDatabase::GetDataInfo( int nIndex, int nColumn )
{
	return m_nDataInfo[nIndex][nColumn];
}

double CDatabase::GetProbabilityUsingGapAcceptance( int nIndex, int nTime )
{
	return CFeatureExtractor::GetInstance()->GetProbabilityUsingGapAcceptance( nIndex, nTime );
}

int CDatabase::LoadStateTransitionMatrix( void )
{
	QString file = "Model/stateTransitionMatrix.csv";

	m_pOpenFile = new QFile( file );

	if( !m_pOpenFile->open(QIODevice::ReadOnly) )
	{
		delete m_pOpenFile;
		return FAIL;
	}

	QTextStream in( m_pOpenFile );
	int nState = 0;

	while( !in.atEnd() )
	{
		QString line = in.readLine();
		QStringList list = line.split(',');

		for( int j=0; j<list.size(); j++ )
		{
			m_dStateTransitionMatrix[nState][j] = list.at(j).toDouble();
		}

		nState++;
	}

	delete m_pOpenFile;

	return DONE;
}

int CDatabase::SaveStateTransitionMatrix( void )
{
	QString file = "Model/stateTransitionMatrix.csv";

	QFile* fp = new QFile( file );
	if( !fp->open(QIODevice::WriteOnly) )
	{
		delete fp;
		return FAIL;
	}

	QTextStream* out = new QTextStream( fp );

	for( int i=0; i<NUM_STATE; i++ )
	{
		for( int j=0; j<NUM_STATE; j++ )
		{
			*out << m_dStateTransitionMatrix[i][j] << ",";
		}
		*out << endl;
	}

	fp->close();

	delete fp;
	delete out;

	return DONE;
}

void CDatabase::SaveModel( stHMM* model, int mode )
{
	QString fileName;

	switch( mode )
	{
	case CHANGING:
		fileName = "Model/" + QString::number( STATE_NUM ) + " states_" + QString::number( FEATURE_VECTOR_DIMENSION ) + " dimension_changing_model.csv";
		break;

	case KEEPING:
		fileName = "Model/" + QString::number( STATE_NUM ) + " states_" + QString::number( FEATURE_VECTOR_DIMENSION ) + " dimension_keeping_model.csv";
		break;
	}

	QFile* fp = new QFile( fileName );
    if( !fp->open(QIODevice::WriteOnly) )
    {
        return;
    }

    QTextStream* out = new QTextStream( fp );

    *out << STATE_NUM << endl;
	*out << model->likelihood << endl;

	for( int i=0; i<STATE_NUM; i++ )
		*out << model->INIT[i] << ",";
	*out << endl;

	for( int i=0; i<STATE_NUM; i++ )
	{
		for( int j=0; j<STATE_NUM; j++ )
			*out << model->TRANS[i][j] << ",";

		*out << endl;
	}

	for( int i=0; i<STATE_NUM; i++ )
		for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
			*out << model->EMIS[i][k][0] << "," << model->EMIS[i][k][1] << endl;

    fp->close();

    delete fp;
    delete out;
}

void CDatabase::LoadModel( stHMM* model, int mode )
{
	QString fileName;

	switch( mode )
	{
	case CHANGING:
		fileName = "Model/" + QString::number( STATE_NUM ) + " states_" + QString::number( FEATURE_VECTOR_DIMENSION ) + " dimension_changing_model.csv";
		break;

	case KEEPING:
		fileName = "Model/" + QString::number( STATE_NUM ) + " states_" + QString::number( FEATURE_VECTOR_DIMENSION ) + " dimension_keeping_model.csv";
		break;
	}

	m_pOpenFile = new QFile( fileName );

    if( !m_pOpenFile->open(QIODevice::ReadOnly) )
    {
        delete m_pOpenFile;
		return;
    }

    QTextStream in( m_pOpenFile );

    int t = 0;
	int k=0;
	int i=0;
    while( !in.atEnd() )
    {
        QString line = in.readLine();
        QStringList list = line.split(',');

		if( t == 1 )
			model->likelihood = list.at(0).toFloat();
		else if( t == 2 )
		{
			for( int j=0; j<list.size(); j++ )
				model->INIT[j] = list.at(j).toFloat();
		}
		else if( t >= 3 && t<(3+STATE_NUM) )
		{
			for( int j=0; j<list.size(); j++ )
				model->TRANS[t-3][j] = list.at(j).toFloat();
		}
		else if( t >= (3+STATE_NUM)  )
		{
			for( int j=0; j<list.size(); j++ )
				model->EMIS[i][k][j] = list.at(j).toFloat();

			k++;
			if( k == FEATURE_VECTOR_DIMENSION )
			{
				k=0;
				i++;
			}
		}

		t++;
    }

	m_pOpenFile->close();
	delete m_pOpenFile;
	
}
///////////////////////////////////////////////////////////////////////////
/* Private member functions */
///////////////////////////////////////////////////////////////////////////
int CDatabase::loadLineFiles( void )
{
	int nReturn = 0;

	double dLineData[NUM_LINE][MAX_LINE_PT][2] = { 0.0 };

	int nNumLine1, nNumLine2, nNumLine3, nNumLine4, nNumLine5;
	nNumLine1 = nNumLine2 = nNumLine3 = nNumLine4 = nNumLine5 = 0;
	
	for( int set = 1; set<4; set++ )
	{
		QString dir = LINE_FILE_PATH;
		dir = dir + "set" + QString::number( set ) + "/";

		for( int n=0; n<MAX_VEHICLE_NO; n++ )
		{
			QString file = dir + "no" + QString::number( n ) + ".csv";
			double dTempData[T_MAX][NUM_COLUMN] = { 0.0 };

			m_pOpenFile = new QFile( file );

			if( !m_pOpenFile->open(QIODevice::ReadOnly) )
			{
				delete m_pOpenFile;
				continue;
			}

			QTextStream in( m_pOpenFile );
			int t = 0;

			while( !in.atEnd() )
			{
				QString line = in.readLine();
				QStringList list = line.split(',');

				for( int j=0; j<list.size(); j++ )
				{
					dTempData[t][j] = list.at(j).toDouble();
				}

				t++;
				if( t > T_MAX ) return FAIL;
			}

			int nStartLane = dTempData[0][DATA_PACKET_LANE];
			int nEndLane = 0;

			for( int k=0; k<t; k++ )
			{
				if( dTempData[k][DATA_PACKET_LANE] != nStartLane )
				{
					nEndLane = dTempData[k][DATA_PACKET_LANE];

					if( (nStartLane == 1 && nEndLane == 2) || (nStartLane == 2 && nEndLane == 1) )
					{
						dLineData[0][nNumLine1][0] = dTempData[k][DATA_PACKET_X];
						dLineData[0][nNumLine1][1] = dTempData[k][DATA_PACKET_Y];
						nNumLine1++;
					}
					else if( (nStartLane == 2 && nEndLane == 3) || (nStartLane == 3 && nEndLane == 2) )
					{
						dLineData[1][nNumLine2][0] = dTempData[k][DATA_PACKET_X];
						dLineData[1][nNumLine2][1] = dTempData[k][DATA_PACKET_Y];
						nNumLine2++;
					}
					else if( (nStartLane == 3 && nEndLane == 4) || (nStartLane == 4 && nEndLane == 3) )
					{
						dLineData[2][nNumLine3][0] = dTempData[k][DATA_PACKET_X];
						dLineData[2][nNumLine3][1] = dTempData[k][DATA_PACKET_Y];
						nNumLine3++;
					}
					else if( (nStartLane == 4 && nEndLane == 5) || (nStartLane == 5 && nEndLane == 4) )
					{
						dLineData[3][nNumLine4][0] = dTempData[k][DATA_PACKET_X];
						dLineData[3][nNumLine4][1] = dTempData[k][DATA_PACKET_Y];
						nNumLine4++;
					}
					else if( (nStartLane == 5 && nEndLane == 6) || (nStartLane == 6 && nEndLane == 5) )
					{
						dLineData[4][nNumLine5][0] = dTempData[k][DATA_PACKET_X];
						dLineData[4][nNumLine5][1] = dTempData[k][DATA_PACKET_Y];
						nNumLine5++;
					}

					break;
				}
			}

			delete m_pOpenFile;
		}

		qDebug() << "database.cpp @ Line data were loaded : set" << QString::number(set) << " - Line1=" << nNumLine1 << ", Line2=" << nNumLine2 << ", Line3=" << nNumLine3
		             << ", Line4=" << nNumLine4 << ", Line5=" << nNumLine5;
	}

	int nNumLinePt[5] = { nNumLine1, nNumLine2, nNumLine3, nNumLine4, nNumLine5 };
	double dCoefficient[5][3] = { 0.0 };
	CLineExtractor::GetInstance()->Extract( nNumLinePt, dLineData, dCoefficient );

	saveLinePt( nNumLinePt, dLineData );
	nReturn = saveApproximateCurve( dCoefficient );

	return nReturn;
}

int CDatabase::saveLinePt( int* pnNumPt, double (*pdData)[500][2] )
{
	for( int n=0; n<NUM_LINE; n++ )
	{
		int nDataLength = pnNumPt[n];

#if defined (US_101)
		QString fileName = "Lane/us101_line" + QString::number(n+1) + ".csv";
#elif (I_80)
		QString fileName = "Lane/i80_line" + QString::number(n+1) + ".csv";
#endif

		QFile* fp = new QFile( fileName );
		if( !fp->open(QIODevice::WriteOnly) )
		{
			return FAIL;
		}

		QTextStream* out = new QTextStream( fp );

		for( int t=0; t<nDataLength; t++ )
		{
			*out << (n+1) << "," << t << "," << pdData[n][t][0] << "," << pdData[n][t][1] << endl;
		}

		fp->close();

		delete fp;
		delete out;
	}

	return DONE;
}

int CDatabase::saveApproximateCurve( double (*pdData)[3] )
{
#if defined (US_101)
	QString fileName = "Lane/us101_approximateCurve.csv";
#elif (I_80)
	QString fileName = "Lane/i80_approximateCurve.csv";
#endif

	QFile* fp = new QFile( fileName );
	if( !fp->open(QIODevice::WriteOnly) )
	{
		delete fp;
		return FAIL;
	}

	QTextStream* out = new QTextStream( fp );

	for( int n=0; n<NUM_LINE; n++ )
	{
		*out << pdData[n][0] << "," << pdData[n][1] << "," << pdData[n][2] << endl;
	}

	fp->close();

	delete fp;
	delete out;

	return DONE;
}

int CDatabase::loadApproximateCurves( void )
{
#if defined (US_101)
	QString file = "Lane/us101_approximateCurve.csv";
#elif (I_80)
	QString file = "Lane/i80_approximateCurve.csv";
#endif

	m_pOpenFile = new QFile( file );

	if( !m_pOpenFile->open(QIODevice::ReadOnly) )
	{
		delete m_pOpenFile;
		return FAIL;
	}

	QTextStream in( m_pOpenFile );
	int nLine = 0;

	while( !in.atEnd() )
	{
		QString line = in.readLine();
		QStringList list = line.split(',');

		for( int j=0; j<list.size(); j++ )
		{
			m_dApproximateCurves[nLine][j] = list.at(j).toDouble();
		}

		nLine++;
		if( nLine > NUM_LINE ) return FAIL;
	}

	CLineExtractor::GetInstance()->Set( m_dApproximateCurves );

	delete m_pOpenFile;
	return DONE;
}

int CDatabase::loadTrafficData( int nMode, int nType )
{
	QString dir = TRAFFIC_FILE_PATH;

	switch( nMode )
	{
	case TRAINER:
		if( nType == KEEPING ) dir = dir + "train_keeping/";
		else	dir = dir + "train_changing/";
		break;

	case TESTER:
		if( nType == KEEPING ) dir = dir + "test_keeping/";
		else dir = dir + "test_changing/";
		break;
	}

	for( int n=0; n<MAX_VEHICLE_NO; n++ )
	{
		QString file = dir + "no" + QString::number( n ) + ".csv";

		m_pOpenFile = new QFile( file );

        if( !m_pOpenFile->open(QIODevice::ReadOnly) )
        {
            delete m_pOpenFile;
            continue;
        }

        QTextStream in( m_pOpenFile );

        int t = 0;
        while( !in.atEnd() )
        {
            QString line = in.readLine();
            QStringList list = line.split(',');

            for( int j=0; j<list.size(); j++ )
            {
                m_dTrafficData[m_nNumTrafficData][t][j] = list.at(j).toDouble();
            }

            t++;

            if( t >= T_MAX )
			{
				qDebug() << "database.cpp @ Traffic data length was overed max value";
                return FAIL;
			}
        }

		int nDataLength = t;
		int nStartLane = m_dTrafficData[m_nNumTrafficData][0][DATA_PACKET_LANE];
		
		if (nType == CHANGING)
		{
			int nGroundTruth = nDataLength;
			for (int tt = 0; tt < nDataLength; tt++)
			{
				if (m_dTrafficData[m_nNumTrafficData][tt][DATA_PACKET_LANE] != nStartLane)
				{
					nGroundTruth = tt;
					break;
				}
			}

			if (nGroundTruth < MEASUREMENT_TIME)
			{
				delete m_pOpenFile;
				continue;
			}
		}
		else // nType == KEEPING
		{
			int nLane = m_dTrafficData[m_nNumTrafficData][0][DATA_PACKET_LANE];

			if (nLane >= 5)
			{
				delete m_pOpenFile;
				continue;
			}
		}

		m_nDataInfo[m_nNumTrafficData][DATA_INFO_PACKET_VEHICLE_NO] = m_dTrafficData[m_nNumTrafficData][0][0]; // vehicle no.
		m_nDataInfo[m_nNumTrafficData][DATA_INFO_PACKET_DATA_LENGTH] = nDataLength; // the data length

		m_nNumTrafficData++;
		if( m_nNumTrafficData >= NUM_TRAFFIC_DATA )
		{
			qDebug() << "database.cpp @ The number of traffic data was overed max value";
			delete m_pOpenFile;
            break; //return FAIL;
		}

		delete m_pOpenFile;
	}

	qDebug() << "database.cpp @ " + QString::number( m_nNumTrafficData ) + " files were loaded...";

	// Save the ground truth to the data information
	for( int n=0; n<m_nNumTrafficData; n++ )
	{
		int nDataLength = m_nDataInfo[n][DATA_INFO_PACKET_DATA_LENGTH];
		int nStartLane = m_dTrafficData[n][0][DATA_PACKET_LANE];

		for( int t=0; t<nDataLength; t++ )
		{
			if( m_dTrafficData[n][t][DATA_PACKET_LANE] != nStartLane )
			{
				m_nDataInfo[n][DATA_INFO_PACKET_GROUND_TRUTH] = t;
				break;
			}
		}
	}

	// Extract features from the traffic data
#if defined (PROPOSED)
	CFeatureExtractor::GetInstance()->Extract( nMode, nType, m_nNumTrafficData, reinterpret_cast<void*>(m_dTrafficData), reinterpret_cast<void*>(m_dFeatureData), reinterpret_cast<void*>(m_nLabelData), reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dStateTransitionMatrix) );

	CFilter::GetInstance()->MovingAverageFilter( FEATURE, m_nNumTrafficData, reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dFeatureData), 0 );
	CFilter::GetInstance()->MovingAverageFilter( FEATURE, m_nNumTrafficData, reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dFeatureData), 1 );
#elif (PROPOSED_PF)
	CFeatureExtractor::GetInstance()->Extract( nMode, nType, m_nNumTrafficData, reinterpret_cast<void*>(m_dTrafficData), reinterpret_cast<void*>(m_dFeatureData), reinterpret_cast<void*>(m_nLabelData), reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dStateTransitionMatrix) );

	CFilter::GetInstance()->MovingAverageFilter( FEATURE, m_nNumTrafficData, reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dFeatureData), 0 );
	CFilter::GetInstance()->MovingAverageFilter( FEATURE, m_nNumTrafficData, reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dFeatureData), 1 );
#elif (MANDALIA)
	CMandalia::GetInstance()->Extract( nMode, nType, m_nNumTrafficData, reinterpret_cast<void*>(m_dTrafficData), reinterpret_cast<void*>(m_dFeatureData), reinterpret_cast<void*>(m_nLabelData), reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dStateTransitionMatrix) );

	CFilter::GetInstance()->MovingAverageFilter( FEATURE, m_nNumTrafficData, reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dFeatureData), 0 );
	CFilter::GetInstance()->MovingAverageFilter( FEATURE, m_nNumTrafficData, reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dFeatureData), 1 );
	CFilter::GetInstance()->MovingAverageFilter( FEATURE, m_nNumTrafficData, reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dFeatureData), 2 );
	CFilter::GetInstance()->MovingAverageFilter( FEATURE, m_nNumTrafficData, reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dFeatureData), 3 );
#elif (SCHLECHTRIEMEN)
	CSchlechtriemen::GetInstance()->Extract( nMode, nType, m_nNumTrafficData, reinterpret_cast<void*>(m_dTrafficData), reinterpret_cast<void*>(m_dFeatureData), reinterpret_cast<void*>(m_nLabelData), reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dStateTransitionMatrix) );

	CFilter::GetInstance()->MovingAverageFilter( FEATURE, m_nNumTrafficData, reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dFeatureData), 0 );
	CFilter::GetInstance()->MovingAverageFilter( FEATURE, m_nNumTrafficData, reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dFeatureData), 1 );
	CFilter::GetInstance()->MovingAverageFilter( FEATURE, m_nNumTrafficData, reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dFeatureData), 2 );
#else
	CFeatureExtractor::GetInstance()->Extract( nMode, nType, m_nNumTrafficData, reinterpret_cast<void*>(m_dTrafficData), reinterpret_cast<void*>(m_dFeatureData), reinterpret_cast<void*>(m_nLabelData), reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dStateTransitionMatrix) );

	CFilter::GetInstance()->MovingAverageFilter( FEATURE, m_nNumTrafficData, reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dFeatureData), 0 );
	CFilter::GetInstance()->MovingAverageFilter( FEATURE, m_nNumTrafficData, reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dFeatureData), 1 );
#endif

	// Filtering feature data
	CFilter::GetInstance()->MedianFilter( m_nNumTrafficData, reinterpret_cast<void*>(m_nDataInfo), reinterpret_cast<void*>(m_dFeatureData), 0 );

	saveData( FEATURE );

	if( nMode == TRAINER )
		SaveStateTransitionMatrix();

	return DONE;
}

int CDatabase::saveData( int nMode )
{
	QString path;
	int nNumData = 0;

	switch( nMode )
	{
	case FEATURE:
		path = "../../Log/Feature/LaneChangeEstimator/";
		nNumData = m_nNumTrafficData;
		break;
	}

	for( int n=0; n<nNumData; n++ )
	{
		int nVehicleNo = m_nDataInfo[n][0];
		int nDataLength = m_nDataInfo[n][1];
		QString file = path + "no" + QString::number( nVehicleNo ) + ".csv";

		QFile* fp = new QFile( file );
		if( !fp->open(QIODevice::WriteOnly) )
		{
			delete fp;
			return FAIL;
		}

		QTextStream* out = new QTextStream( fp );

		for( int t=0; t<nDataLength; t++ )
		{
			for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
			{
				*out << m_dFeatureData[n][t][k] << ",";
			}
			*out << endl;
		}

		fp->close();

		delete fp;
		delete out;
	}

	return DONE;
}