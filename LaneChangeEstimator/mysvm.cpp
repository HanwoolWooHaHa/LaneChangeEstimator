/**
* @file	mysvm.cpp
* @version	1.00
* @author	Hanwool Woo
* @date	Creation date: 2016/01/16
* @brief	this file is for the management of data files
*/
#include "mysvm.h"
#include "defines.h"
#include "database.h"
#include "featureExtractor.h"

#include <qdebug.h>
#include <qfile.h>
#include <qtextstream.h>
#include <time.h>

CMySvm::CMySvm()
{
	m_pModel = NULL;
	memset( m_dProbabiliy, 0, sizeof( double ) * NUM_STATE * T_MAX );

	m_nParamCost = 8;
}

CMySvm::~CMySvm()
{
	delete m_pModel;
}
///////////////////////////////////////////////////////////////////////////
/* Public functions */
///////////////////////////////////////////////////////////////////////////
void CMySvm::Train( int nType )
{
	qDebug() << "mysvm.cpp @ SVM was selected for the estimator";

	svm_parameter param;

	// 1. set the parameter of Support Vector Machine
	setParameter(&param);

	// 2. generate the packet data for the training using Support Vector Machine
	makePacketForSVM();

	// 3. check training data
    int nDataLength = m_pt_list.size();
    if( nDataLength == 0 )
	{
		qDebug() << "mysvm.cpp @ Training data were not successfully loaded...";
        return;
	}

    // 4. generate the problem for using Support Vector Machine
    svm_problem problem;
    problem.l = m_pt_list.size();
    problem.y = new double[problem.l];

	svm_node* x_space = new svm_node[ (FEATURE_VECTOR_DIMENSION * WINDOW_SIZE + 1) * nDataLength ];
    problem.x = new svm_node * [problem.l];

	long i=0;
    for (std::list <stFeatureVector>::iterator q = m_pt_list.begin(); q != m_pt_list.end(); q++, i++)
    {
        int dim = FEATURE_VECTOR_DIMENSION * WINDOW_SIZE + 1;

		for (int k = 0; k<(FEATURE_VECTOR_DIMENSION * WINDOW_SIZE); k++)
		{
			x_space[dim * i + k].index = k+1;
			x_space[dim * i + k].value = q->x[k];
		}

		x_space[dim * i + FEATURE_VECTOR_DIMENSION * WINDOW_SIZE].index = -1;

        problem.x[i] = &x_space[dim * i];
        problem.y[i] = q->label;
    }

	if( svm_check_parameter(&problem, &param) != NULL )
    {
        qDebug() << "mysvm.cpp @ model has some errors";
        return;
    }

	/*******************************************************/
	clock_t startTime = clock();
	// 5. traing the svm model using feature vectors
	qDebug() << "mysvm.cpp @ Start to train the svm model!!!!";
    m_pModel = svm_train(&problem, &param);

	clock_t endTime = clock();
	qDebug() << "Training calculation time : " << ( endTime - startTime ) / CLOCKS_PER_SEC << "sec";
	qDebug() << "Training calculation time : " << ( endTime - startTime ) / CLOCKS_PER_SEC / 60 << "min" << endl;
	/*******************************************************/

	// 6. save trained model
	saveModel( m_pModel );

	svm_free_and_destroy_model( &m_pModel );
    delete[] x_space;
    delete[] problem.x;
    delete[] problem.y;
    free( param.weight_label );
    free( param.weight );
}

void CMySvm::Test( void )
{
	// 1. load the trained model from a file
	//loadModel( m_pModel );

#if defined(PROPOSED)
	QString modelFile = "Model/Proposed.txt";
#elif (MANDALIA)
	QString modelFile = "Model/Mandalia.txt";
#elif (SCHLECHTRIEMEN)
	QString modelFile = "Model/Schlechtriemen.txt";
#elif (PROPOSED_PF)
	//QString modelFile = "Model/Proposed(PF).txt";
	QString modelFile = "Model/160324_Proposed(SVM).txt";
#else
	QString modelFile = "Model/Model_K" + QString::number( FEATURE_VECTOR_DIMENSION ) + "_C" + QString::number( m_nParamCost ) + ".txt";
#endif
	
	std::string cStr = modelFile.toLocal8Bit();
	int nLen = cStr.length();
	char* chr = new char [nLen+1];
	memcpy( chr, cStr.c_str(), nLen+1 );
	
	m_pModel = svm_load_model( chr );

	delete [] chr;
	if( m_pModel == NULL )
	{
		qDebug() << "mysvm.cpp @ SVM model cannot be loaded";
		return;
	}

	// 2. make the packet data for the use to SVM
	makePacketForSVM();

	svm_node x[FEATURE_VECTOR_DIMENSION * WINDOW_SIZE + 1];
	for (int k = 0; k<FEATURE_VECTOR_DIMENSION * WINDOW_SIZE; k++)
		x[k].index = k+1;
	x[FEATURE_VECTOR_DIMENSION * WINDOW_SIZE].index = -1;

	int n = 0;
	int t = 0;
	int nListLength = m_pt_list.size();
	double* pdProbability = new double [NUM_STATE];

	for ( std::list <stFeatureVector>::iterator q = m_pt_list.begin(); q != m_pt_list.end(); q++ )
    {
		int nVehicleNo = CDatabase::GetInstance()->GetDataInfo( n, DATA_INFO_PACKET_VEHICLE_NO );
		int nDataLength = CDatabase::GetInstance()->GetDataInfo( n, DATA_INFO_PACKET_DATA_LENGTH );

		for (int k = 0; k<(FEATURE_VECTOR_DIMENSION * WINDOW_SIZE); k++)
			x[k].value = q->x[k];

		double dResult = svm_predict_probability( m_pModel, x, pdProbability );
		double dProbabilityUsingGapAcceptance = CFeatureExtractor::GetInstance()->GetProbabilityUsingGapAcceptance( n, t );

#if defined (BAYESIAN_FILTER)
		dResult = bayesianFilter( t, dResult, pdProbability );
#endif

		q->label = (signed char)dResult;
		q->prob = pdProbability[1] * dProbabilityUsingGapAcceptance;

		judge( n, t, dResult, pdProbability );
	}

	saveResult();

	svm_free_and_destroy_model( &m_pModel );
	delete [] pdProbability;
}
///////////////////////////////////////////////////////////////////////////
/* Private member functions */
///////////////////////////////////////////////////////////////////////////
void CMySvm::setParameter( svm_parameter* param )
{
	param->svm_type = C_SVC;
	param->kernel_type = RBF; //LINEAR;//RBF;
    param->degree = 3; //! for poly
    param->gamma = 0.0625; /* for poly/rbf/sigmoid */
    param->coef0 = 1; /* for poly/sigmoid */

    /* these are for training only */
    param->nu = 0.2; /* for NU_SVC, ONE_CLASS, and NU_SVR */
    param->cache_size = 1024; /* in MB */
    param->C = m_nParamCost; /* for C_SVC, EPSILON_SVR, and NU_SVR */
    param->eps = 1e-3; /* stopping criteria */
    param->p = 0.1; /* for EPSILON_SVR */
    param->shrinking = 0; /* use the shrinking heuristics */
    param->probability = 1; /* do probability estimates */
    param->nr_weight = 0; /* for C_SVC */
    param->weight_label = NULL; /* for C_SVC */
    param->weight = NULL; /* for C_SVC */
}

int CMySvm::makePacketForSVM( void )
{
	int nNumData = CDatabase::GetInstance()->GetNumData();
	
	for( int n=0; n<nNumData; n++ )
	{
		int nDataLength = CDatabase::GetInstance()->GetDataInfo( n, DATA_INFO_PACKET_DATA_LENGTH );

		for( int t=0; t<nDataLength; t++ )
		{
			registerVectorToList( n, t );
		}
	}

	return DONE;
}

int CMySvm::registerVectorToList( int nIndex, int nT )
{
	stFeatureVector p;

	/*if (nT < (WINDOW_SIZE - 1))
	{
		return DONE;
	}*/

	for (int k = 0; k < FEATURE_VECTOR_DIMENSION; k++)
	{
		if (nT >= (WINDOW_SIZE - 1))
		{
			for (int n = 0; n < WINDOW_SIZE; n++)
				p.x[k * WINDOW_SIZE + n] = CDatabase::GetInstance()->GetData(FEATURE, nIndex, nT-n, k);
		}
		else
		{
			for (int n = 0; n < WINDOW_SIZE; n++ ) // 同じ特徴量をWindow size回数入れる
				p.x[k * WINDOW_SIZE + n] = CDatabase::GetInstance()->GetData(FEATURE, nIndex, nT, k);
		}
	}

	// 現時刻Tにおけるラベルを特徴ベクトルのラベルとする
	p.label = CDatabase::GetInstance()->GetData( LABEL, nIndex, nT );

	m_pt_list.push_back( p );

	return DONE;
}

bool CMySvm::judge( int& nDataIndex, int& nTime, double dResult, double* dProb )
{
	static bool flag = false;
	static double dSum = 0.0;
	static int nFalseAlarm = 0;
	static int nFalseAlarmList[100];
	static int nFailed = 0;
	static int nFailedCaseList[100];
	static int nGapList[NUM_TRAFFIC_DATA] = { 0 };
	static int nNumGap = 0;

	int nNumData = CDatabase::GetInstance()->GetNumData();
	int nVehicleNo = CDatabase::GetInstance()->GetDataInfo( nDataIndex, DATA_INFO_PACKET_VEHICLE_NO );
	int nDataLength = CDatabase::GetInstance()->GetDataInfo( nDataIndex, DATA_INFO_PACKET_DATA_LENGTH );
	double dProbabilityUsingGapAcceptance = CFeatureExtractor::GetInstance()->GetProbabilityUsingGapAcceptance( nDataIndex, nTime );

	if( (int)dResult == LANE_CHANGING && flag == false )
	{
		int nGroundTruth = CDatabase::GetInstance()->GetDataInfo( nDataIndex, DATA_INFO_PACKET_GROUND_TRUTH );
		int nGap = nGroundTruth - nTime;

		if( nGap > PREDICTION_TIME_LIMIT )
		{
			nFalseAlarmList[nFalseAlarm] = nVehicleNo;
			nFalseAlarm++;
		}
		else if( nGap <= 0 )
		{
			nFailedCaseList[nFailed] = nVehicleNo;
			nFailed++;
		}
		else
		{
			dSum += nGap;
			nGapList[nNumGap] = nGap;
			nNumGap++;
		}

		qDebug() << endl << "Vehicle No." << nVehicleNo << " - Gap : " << nGap;
		flag = true;
	}

	nTime++;

	if( nTime == nDataLength )
	{
		if( flag == false )
		{
			nFailedCaseList[nFailed] = nVehicleNo;
			nFailed++;
		}

		nDataIndex++;
		nTime = 0;
		flag = false;
		memset( m_dProbabiliy, 0, sizeof( double ) * NUM_STATE * T_MAX );

		if( nDataIndex == nNumData )
		{
			dSum = dSum / ( nNumData - nFalseAlarm - nFailed ) * 0.1;
			qDebug() << endl << "False alarm = " << nFalseAlarm << ",  Failed = " << nFailed << ",  Avg = " << dSum;

			for( int n=0; n<nFalseAlarm; n++ )
				qDebug() << "False alarm Vehicle No." << nFalseAlarmList[n];

			for( int n=0; n<nFailed; n++ )
				qDebug() << "Failed Vehicle No." << nFailedCaseList[n];

			double dStd = 0.0;
			double dAvg = dSum * 10;
			for( int n=0; n<nNumGap; n++ )
			{
				dStd += ( nGapList[n] - dAvg ) * ( nGapList[n] - dAvg );
			}
			dStd = dStd / nNumGap;
			qDebug() << "Standard Deviation = " << dStd;
		}
	}

	return false;
}

double CMySvm::bayesianFilter( int nTime, double dResult, double* dProb )
{
	if( nTime == 0 )
	{
		for( int i=0; i<NUM_STATE; i++ )
			m_dProbabiliy[i][nTime] = dProb[i];
	}
	else
	{
		for( int i=0; i<NUM_STATE; i++ )
		{
			double dSum = 0.0;
			for( int j=0; j<NUM_STATE; j++ )
			{
				dSum += CDatabase::GetInstance()->GetStateTransitionMatrix( j, i ) * m_dProbabiliy[j][nTime-1];
			}
			m_dProbabiliy[i][nTime] = dProb[i] * dSum;
		}
	}

	if( dResult==LANE_CHANGING && m_dProbabiliy[1][nTime] <= m_dProbabiliy[0][nTime] )
		dResult = LANE_KEEPING;

	return dResult;
}

void CMySvm::saveResult( void )
{
	QString path = "../../Log/Result/";

	int nDataIndex = 0;
	int nTime = 0;

	QFile* fp;
	QTextStream* out;

	for ( std::list <stFeatureVector>::iterator q = m_pt_list.begin(); q != m_pt_list.end(); q++ )
	{
		int nVehicleNo = CDatabase::GetInstance()->GetDataInfo( nDataIndex, DATA_INFO_PACKET_VEHICLE_NO );
		int nDataLength = CDatabase::GetInstance()->GetDataInfo( nDataIndex, DATA_INFO_PACKET_DATA_LENGTH );
		
		if( nTime == 0 )
		{
			QString file = path + QString::number( nVehicleNo ) + ".csv";

			fp = new QFile( file );
			if( !fp->open(QIODevice::WriteOnly) )
			{
				delete fp;
				return;
			}

			out = new QTextStream( fp );
		}

		*out << nVehicleNo << "," << nTime << ",";
		for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
			*out << q->x[k] << ",";
		*out << q->label << "," << q->prob << endl;

		nTime++;

		if( nTime == nDataLength )
		{
			nDataIndex++;
			nTime = 0;

			fp->close();

			delete fp;
			delete out;
		}
	}
}

void CMySvm::saveModel( svm_model* model )
{
#if 0
	time_t now = time( NULL );
	struct tm *pNow = localtime( &now );
	int nYear = pNow->tm_year+1900;
	int nMonth = pNow->tm_mon+1;
	int nDay = pNow->tm_mday;
	int nHour = pNow->tm_hour;
	int nMin = pNow->tm_min;
#endif

	QString modelFile = "Model/Model_K" + QString::number( FEATURE_VECTOR_DIMENSION ) + "_C" + QString::number( m_nParamCost ) + ".txt";
	std::string cStr = modelFile.toLocal8Bit();
	int nLen = cStr.length();
	char* chr = new char [nLen+1];
	memcpy( chr, cStr.c_str(), nLen+1 );

	if( svm_save_model(chr, m_pModel) == 0 )
    {
        qDebug() << "mysvm.cpp @ trained model was successfully saved";
    }
    else
    {
        qDebug() << "mysvm.cpp @ It was failed to save the trained model";
    }
}

void CMySvm::loadModel( svm_model* pModel )
{
	QString modelFile = "Model/Model_K" + QString::number( FEATURE_VECTOR_DIMENSION ) + "_C" + QString::number( m_nParamCost ) + ".txt";

	std::string cStr = modelFile.toLocal8Bit();
	int nLen = cStr.length();
	char* chr = new char [nLen+1];
	memcpy( chr, cStr.c_str(), nLen+1 );
	
	pModel = svm_load_model( chr );

	delete [] chr;
}