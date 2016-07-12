/**
* @file	myHmm.cpp
* @version	1.00
* @author	Hanwool Woo
* @date	Creation date: 2016/02/01
* @brief	this file is for the management of data files
*/
#include "myHmm.h"
#include "database.h"
#include "Library\libhmm\baumWelch.h"
#include "Library\libhmm\viterbi.h"

#include <QTime>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <time.h>

CMyHmm::CMyHmm()
{
	m_pHmm = new stHMM;
	m_pBaum = new CBaumWelch();

	initialize();

	m_pdResult = new double [NUM_TRAFFIC_DATA];
}

CMyHmm::~CMyHmm()
{
	delete m_pHmm;
	delete m_pBaum;

	delete [] m_pdResult;
}

void CMyHmm::Train( int nType )
{
	int ITER_MAX = 1000;
	stHMM stMaxHmm = *m_pHmm;
	double dMaxLikelihood = 0.0;

    for( int iter=0; iter<ITER_MAX; iter++ )
    {
		stHMM stPreHmm = *m_pHmm;

		int nReturn = m_pBaum->Train( m_pHmm );

		if( nReturn != 0 )
		{
			qDebug() << "myHmm.cpp @ The returned value of Baum Welch has some errors!!";
			break;
		}

		qDebug() << "myHmm.cpp @ train no." << iter << " :: Likelihood = " << m_pHmm->likelihood;

		if( m_pHmm->likelihood != m_pHmm->likelihood )
		{
			qDebug() << "myHmm.cpp @ Model's likelihood is NAND";
			*m_pHmm = stPreHmm;
			//break;

			initialize();
			continue;
		}

		if( m_pHmm->likelihood > dMaxLikelihood )
		{
			dMaxLikelihood = m_pHmm->likelihood;
			stMaxHmm = *m_pHmm;
		}

		double dDelta = m_pHmm->likelihood - stPreHmm.likelihood;
		if( dDelta < 0.01 )
		{
			initialize();
		}
	}

	printModel( &stMaxHmm );

	CDatabase::GetInstance()->SaveModel( &stMaxHmm, nType );
}

void CMyHmm::Test( void )
{
	clock_t startTime = clock();
	m_nSumCalcTime = 0;

	/*if( STATE_NUM == 3 )
		testUsingStateHmm();
	else
		testUsingModelHmm();*/

	testUsingStateHmm();

	clock_t endTime = clock();
	qDebug() << "Training calculation time : " << ( endTime - startTime ) << "ms";
	//qDebug() << "Sum of Calc Time = " << m_nSumCalcTime << "ms / data";
	qDebug() << "Sum of Calc Time = " << ( endTime - startTime ) / (double)m_nSumCalcTime << "ms / data";
}
///////////////////////////////////////////////////////////////////////////
/* Private member functions */
///////////////////////////////////////////////////////////////////////////
void CMyHmm::initialize( void )
{
	qsrand((uint)QTime::currentTime().msec());

    double rnd[STATE_NUM][FEATURE_VECTOR_DIMENSION];

    for( int i=0; i<STATE_NUM; i++ )
    {
        for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
        {
            rnd[i][k] = qrand() % 628;
            rnd[i][k] /= 100;

            rnd[i][k] = sin( rnd[i][k] );
        }
    }

    //! 1. Initialize hidden Markov models
    memset( m_pHmm->INIT, 0, sizeof(float) * STATE_NUM );
    memset( m_pHmm->TRANS, 0, sizeof(float) * STATE_NUM * STATE_NUM );
    memset( m_pHmm->EMIS, 0, sizeof(float) * STATE_NUM * FEATURE_VECTOR_DIMENSION * 2 );

    m_pHmm->likelihood = 0.0;

    for( int i=0; i<STATE_NUM; i++ )
    {
        m_pHmm->INIT[i] = 1.0 / STATE_NUM;
    }

    for( int i=0; i<STATE_NUM; i++ )
        for( int j=0; j<STATE_NUM; j++ )
            m_pHmm->TRANS[i][j] = 1.0 / STATE_NUM;

	for( int i=0; i<STATE_NUM; i++ )
    {
        for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
        {
            m_pHmm->EMIS[i][k][0] = rnd[i][k]; // 平均はランダムで設定する
            m_pHmm->EMIS[i][k][1] = 1.0; // 標準偏差は 1 に固定する
        }
    }

	//if( STATE_NUM == 3 ) // HMMの状態数が3の場合のみ一定の値を与える
	//{
	//	// 白線からの距離における平均値を設定
	//	m_pHmm->EMIS[0][0][0] = 1.0;
	//	m_pHmm->EMIS[1][0][0] = 0.0;
	//	m_pHmm->EMIS[2][0][0] = -1.0;

	//	// PFにおける平均値を設定
	//	m_pHmm->EMIS[0][2][0] = 0.0;
	//	m_pHmm->EMIS[1][2][0] = 1.0;
	//	m_pHmm->EMIS[2][2][0] = 0.0;

	//	m_pHmm->TRANS[0][0] = 0.5;
	//	m_pHmm->TRANS[0][1] = 0.5;
	//	m_pHmm->TRANS[0][2] = 0.0;

	//	m_pHmm->TRANS[1][0] = 0.0;
	//	m_pHmm->TRANS[1][1] = 0.5;
	//	m_pHmm->TRANS[1][2] = 0.5;

	//	m_pHmm->TRANS[2][0] = 0.0;
	//	m_pHmm->TRANS[2][1] = 0.0;
	//	m_pHmm->TRANS[2][2] = 1.0;
	//}
}

void CMyHmm::printModel( stHMM* model )
{
    qDebug() << endl;

    qDebug() << "myHmm.cpp @ " << "Likelihood = " << model->likelihood;
    for( int i=0; i<STATE_NUM; i++ )
		qDebug() << "myHmm.cpp @ INIT[" << i << "] = " << model->INIT[i];

    for( int i=0; i<STATE_NUM; i++ )
        for( int j=0; j<STATE_NUM; j++ )
            qDebug() << "myHmm.cpp @ " << "TRANS : " << i << " to " << j << " = " << model->TRANS[i][j];

    for( int i=0; i<STATE_NUM; i++ )
		for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
			qDebug() << "myHmm.cpp @ EMIS[" << i << "][" << k << "] : AVG = " << model->EMIS[i][k][0] << ",  STD = " << model->EMIS[i][k][1];

    qDebug() << endl;
}

void CMyHmm::testUsingStateHmm( void )
{
	CDatabase::GetInstance()->LoadModel( m_pHmm, CHANGING );

	printModel( m_pHmm );

	int nNumData = CDatabase::GetInstance()->GetNumData();
	double dSum = 0.0;
	int nCount = 0;
	int nFailed = 0;
	int nFalseAlarm = 0;
	for( int n=0; n<nNumData; n++ )
	{
		int nVehicleNo = CDatabase::GetInstance()->GetDataInfo( n, DATA_INFO_PACKET_VEHICLE_NO );
		int nDataLength = CDatabase::GetInstance()->GetDataInfo( n, DATA_INFO_PACKET_DATA_LENGTH );
		int nGroundTruth = CDatabase::GetInstance()->GetDataInfo( n, DATA_INFO_PACKET_GROUND_TRUTH );
		int nEstimatedTime = 0;
		int nDelta = 0;
		CViterbi::GetInstance()->ResetStateSeq();

		// 推定結果を記録する配列を生成する
		int* pnStateArray = new int[nDataLength];

		for( int t=0; t<nDataLength; t++ )
		{
			// Viterbiアルゴリズムによる現時刻における状態推定
			double dLikelihood = 0.0;
			pnStateArray[t] = CViterbi::GetInstance()->Estimate( n, t, m_pHmm, &dLikelihood );
			
			// 提案手法より「CHANGING」と判断された時間を記録する
			if (pnStateArray[t] == STATE_CHANGING && nEstimatedTime == 0)
			{
				nEstimatedTime = t;
			}

			m_nSumCalcTime++;
		}

		// 推定時間差を算出する
		nDelta = nGroundTruth - nEstimatedTime;

		// 推定時間差から推定の成敗を判断する
		if (nDelta > 0 && nDelta < PREDICTION_TIME_LIMIT)
		{
			dSum += nDelta;
			nCount++;
			qDebug() << "myHmm.cpp @ Success Data no." << nVehicleNo << " - Delta = " << nGroundTruth - nEstimatedTime;
		}
		else if (nDelta <= 0)
		{
			nFailed++;
			qDebug() << "myHmm.cpp @ Failed Data no." << nVehicleNo << " - Delta = " << nGroundTruth - nEstimatedTime;
		}
		else if (nDelta >= PREDICTION_TIME_LIMIT)
		{
			nFalseAlarm++;
			qDebug() << "myHmm.cpp @ False alarm Data no." << nVehicleNo << " - Delta = " << nGroundTruth - nEstimatedTime;
		}

		// 推定結果をファイルに記録する
		saveResult(nVehicleNo, n, pnStateArray);

		// 推定結果配列のメモリを解除する
		delete[] pnStateArray;
	}

	qDebug() << endl << "myHmm.cpp @ Avg. Est. Time = " << dSum / nCount / 10;
	qDebug() << endl << "myHmm.cpp @ Failed = " << nFailed;
	qDebug() << endl << "myHmm.cpp @ False alarm = " << nFalseAlarm;
}

void CMyHmm::testUsingModelHmm( void )
{
	stHMM modelChanging;
	stHMM modelKeeping;

	CDatabase::GetInstance()->LoadModel( &modelChanging, CHANGING );
	CDatabase::GetInstance()->LoadModel( &modelKeeping, KEEPING );

	printModel( &modelChanging );
	printModel( &modelKeeping );

	int nNumData = CDatabase::GetInstance()->GetNumData();
	double dSum = 0.0;
	int nCount = 0;
	int nFailed = 0;
	int nFalseAlarm = 0;
	for( int n=0; n<nNumData; n++ )
	{
		int nVehicleNo = CDatabase::GetInstance()->GetDataInfo( n, DATA_INFO_PACKET_VEHICLE_NO );
		int nDataLength = CDatabase::GetInstance()->GetDataInfo( n, DATA_INFO_PACKET_DATA_LENGTH );
		int nGroundTruth = CDatabase::GetInstance()->GetDataInfo( n, DATA_INFO_PACKET_GROUND_TRUTH );
		int nEstimatedTime = 0;
		int nDelta = 0;
		CViterbi::GetInstance()->ResetStateSeq();

		//qDebug() << "myHmm.cpp @ Ground Truth = " << nGroundTruth;

		for( int t=0; t<nDataLength; t++ )
		{
			double dLikelihood[2] = { 0.0 };

			CViterbi::GetInstance()->Estimate( n, t, &modelChanging, &dLikelihood[0] );
			CViterbi::GetInstance()->Estimate( n, t, &modelKeeping, &dLikelihood[1] );
			
			nEstimatedTime = t;
			double rambda = dLikelihood[0] - dLikelihood[1];

			//qDebug() << "myHmm.cpp @ Data no." << nVehicleNo << "  time = " << t << ",  L1 = " << dLikelihood[0] << ",  L2 = " << dLikelihood[1];

			if( rambda >= 100.0 )
			{
				//qDebug() << "myHmm.cpp @ Data no." << nVehicleNo << "  time = " << t << ",  rambda = " << rambda;
				m_pdResult[n] = rambda;

				break;
			}
		}

		nDelta = nGroundTruth - nEstimatedTime;
		qDebug() << "myHmm.cpp @ Data no." << nVehicleNo << " - Delta = " << nGroundTruth - nEstimatedTime;

		if( nDelta > 0 && nDelta < 51 )
		{
			dSum += nDelta;
			nCount++;
		}
		else if( nDelta <= 0 )
			nFailed++;
		else if( nDelta >= 51 )
			nFalseAlarm++;
	}

	saveResult();

	qDebug() << endl << "myHmm.cpp @ Avg. Est. Time = " << dSum / nCount / 10;
	qDebug() << endl << "myHmm.cpp @ Failed = " << nFailed;
	qDebug() << endl << "myHmm.cpp @ False alarm = " << nFalseAlarm;
}

void CMyHmm::saveResult( void )
{
	QString file = "../../Log/Result/result.csv";

	int nNumData = CDatabase::GetInstance()->GetNumData();

	QFile* fp = new QFile( file );
	
	if( !fp->open(QIODevice::WriteOnly) )
	{
		delete fp;
		return;
	}

	QTextStream* out = new QTextStream( fp );

	for( int n=0; n<nNumData; n++ )
	{
		int nVehicleNo = CDatabase::GetInstance()->GetDataInfo( n, DATA_INFO_PACKET_VEHICLE_NO );
		int nDataLength = CDatabase::GetInstance()->GetDataInfo( n, DATA_INFO_PACKET_DATA_LENGTH );
		
		*out << m_pdResult[n] << endl;
	}

	fp->close();

	delete fp;
	delete out;
}

void CMyHmm::saveResult(int nVehicleNo, int nIndex, int* pnStateArray)
{
	// File名を指定する
	QString file = "../../Log/Result/LaneChangeEstimator/" + QString::number(nVehicleNo) + ".csv";

	QFile* fp = new QFile( file );
	if( !fp->open(QIODevice::WriteOnly) )
	{
		delete fp;
		return;
	}

	QTextStream* out = new QTextStream( fp );

	// データの長さを取得する
	int nDataLength = CDatabase::GetInstance()->GetDataInfo(nIndex, DATA_INFO_PACKET_DATA_LENGTH);

	for (int t = 0; t < nDataLength; t++)
	{
		*out << t << ",";
		double dFeature[FEATURE_VECTOR_DIMENSION] = { 0.0 };
		for (int k = 0; k < FEATURE_VECTOR_DIMENSION; k++)
		{
			dFeature[k] = CDatabase::GetInstance()->GetData(FEATURE, nIndex, t, k);
			*out << dFeature[k] << ",";
		}

		int nState = pnStateArray[t];

		switch (nState)
		{
		case 1: nState = 4; break;
		case 4: nState = 1; break;
		}
		
		*out << nState << endl;
	}

	fp->close();

	delete fp;
	delete out;
}