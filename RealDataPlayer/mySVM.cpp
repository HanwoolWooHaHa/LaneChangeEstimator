/**
* @file	mysvm.cpp
* @version	1.00
* @author	Hanwool Woo
* @date	Creation date: 2016/05/12
* @brief	this file is for the estimation using a support vector machine
*/
#include "mysvm.h"
#include "constant.h"
#include "database.h"

#include <qdebug.h>
#include <qfile.h>
#include <qtextstream.h>
#include <time.h>

CMySvm::CMySvm()
{
	m_pModel = NULL;
	memset(m_dProbabiliy, 0, sizeof(double)* NUM_CLASS * T_MAX);

	m_nParamCost = 8;

	loadModel(m_pModel);
}

CMySvm::~CMySvm()
{
	svm_free_and_destroy_model(&m_pModel);
	delete m_pModel;
}
///////////////////////////////////////////////////////////////////////////
/* Public functions */
///////////////////////////////////////////////////////////////////////////
void CMySvm::Train(int nType)
{
	
}

bool CMySvm::Test(int nTick, int nTargetIndex)
{
	bool bEstimationResult = false;
	double dClassificationResult = -1;

	//  1. make the packet data for the use to SVM
	makePacketForSVM(nTick, nTargetIndex);

	svm_node x[FEATURE_VECTOR_DIMENSION * WINDOW_SIZE + 1];
	for (int k = 0; k<FEATURE_VECTOR_DIMENSION * WINDOW_SIZE; k++)
		x[k].index = k + 1;
	x[FEATURE_VECTOR_DIMENSION * WINDOW_SIZE].index = -1;

	int nListLength = m_feature_vector_list.size();
	double* pdProbability = new double[NUM_CLASS];

	for (std::list <stFeatureVector>::iterator q = m_feature_vector_list.begin(); q != m_feature_vector_list.end(); q++)
	{
		for (int k = 0; k<(FEATURE_VECTOR_DIMENSION * WINDOW_SIZE); k++)
			x[k].value = q->x[k];

		dClassificationResult = svm_predict_probability(m_pModel, x, pdProbability);
		dClassificationResult = bayesianFilter(nTick, dClassificationResult, pdProbability);

		q->label = (signed char)dClassificationResult;
		q->prob = pdProbability[1];

		//bEstimationResult = judge(n, t, dClassificationResult, pdProbability);
	}

	CDatabase::GetInstance()->SetEstimationResultData(nTick, nTargetIndex, RESULT_PACKET_FLAG, (double)bEstimationResult);
	CDatabase::GetInstance()->SetEstimationResultData(nTick, nTargetIndex, RESULT_PACKET_CLASS, dClassificationResult);

	delete[] pdProbability;

	return bEstimationResult;
}
///////////////////////////////////////////////////////////////////////////
/* Private member functions */
///////////////////////////////////////////////////////////////////////////
void CMySvm::setParameter(svm_parameter* param)
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

void CMySvm::makePacketForSVM(int nTick, int nTargetIndex)
{
	double dFeatureDistance = CDatabase::GetInstance()->GetFeatureData(nTargetIndex, nTick, FEATURE_PACKET_DISTANCE);
	double dFeatureLateralVelocity = CDatabase::GetInstance()->GetFeatureData(nTargetIndex, nTick, FEATURE_PACKET_LAT_VELOCITY);
	double dFeaturePotential = CDatabase::GetInstance()->GetFeatureData(nTargetIndex, nTick, FEATURE_PACKET_POTENTIAL);

	if (isnan(dFeatureDistance) || isnan(dFeatureLateralVelocity) || isnan(dFeaturePotential))
		return;

	//! Feature vector listを初期化する
	m_feature_vector_list.clear();

	//! 特徴量ベクトル
	stFeatureVector p;

	if (nTick >= (WINDOW_SIZE-1))
	{
		//! Window内にNANDがあるかをチェックする
		bool bNandFlag = false;
		for (int k = 0; k < FEATURE_VECTOR_DIMENSION; k++)
		{
			for (int n = 0; n < WINDOW_SIZE; n++)
			{
				double dValue = CDatabase::GetInstance()->GetFeatureData(nTargetIndex, nTick - n, k);

				if (isnan(dValue))
				{
					bNandFlag = true;
					break;
				}
			}

			if (bNandFlag)
				break;
		}

		if (bNandFlag) //! 同じ特徴量をWindow size回数入れる
		{
			for (int k = 0; k < FEATURE_VECTOR_DIMENSION; k++)
			{
				for (int n = 0; n < WINDOW_SIZE; n++)
				{
					p.x[k * WINDOW_SIZE + n] = CDatabase::GetInstance()->GetFeatureData(nTargetIndex, nTick, k);
				}
			}
		}
		else
		{
			for (int k = 0; k < FEATURE_VECTOR_DIMENSION; k++)
			{
				for (int n = 0; n < WINDOW_SIZE; n++)
				{
					p.x[k * WINDOW_SIZE + n] = CDatabase::GetInstance()->GetFeatureData(nTargetIndex, nTick - n, k);
				}
			}
		}
	}
	else //! 同じ特徴量をWindow size回数入れる
	{
		for (int k = 0; k < FEATURE_VECTOR_DIMENSION; k++)
		{
			for (int n = 0; n < WINDOW_SIZE; n++)
			{
				p.x[k * WINDOW_SIZE + n] = CDatabase::GetInstance()->GetFeatureData(nTargetIndex, nTick, k);
			}
		}
	}

	//! 特徴量Listに追加する
	m_feature_vector_list.push_back(p);
}


bool CMySvm::judge(int& nDataIndex, int& nTime, double dResult, double* dProb)
{
#if 0
	static bool flag = false;
	static double dSum = 0.0;
	static int nFalseAlarm = 0;
	static int nFalseAlarmList[100];
	static int nFailed = 0;
	static int nFailedCaseList[100];
	static int nGapList[NUM_TRAFFIC_DATA] = { 0 };
	static int nNumGap = 0;

	int nNumData = CDatabase::GetInstance()->GetNumData();
	int nVehicleNo = CDatabase::GetInstance()->GetDataInfo(nDataIndex, DATA_INFO_PACKET_VEHICLE_NO);
	int nDataLength = CDatabase::GetInstance()->GetDataInfo(nDataIndex, DATA_INFO_PACKET_DATA_LENGTH);
	double dProbabilityUsingGapAcceptance = CFeatureExtractor::GetInstance()->GetProbabilityUsingGapAcceptance(nDataIndex, nTime);

	if ((int)dResult == LANE_CHANGING && flag == false)
	{
		int nGroundTruth = CDatabase::GetInstance()->GetDataInfo(nDataIndex, DATA_INFO_PACKET_GROUND_TRUTH);
		int nGap = nGroundTruth - nTime;

		if (nGap > PREDICTION_TIME_LIMIT)
		{
			nFalseAlarmList[nFalseAlarm] = nVehicleNo;
			nFalseAlarm++;
		}
		else if (nGap <= 0)
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

	if (nTime == nDataLength)
	{
		if (flag == false)
		{
			nFailedCaseList[nFailed] = nVehicleNo;
			nFailed++;
		}

		nDataIndex++;
		nTime = 0;
		flag = false;
		memset(m_dProbabiliy, 0, sizeof(double)* NUM_CLASS * T_MAX);

		if (nDataIndex == nNumData)
		{
			dSum = dSum / (nNumData - nFalseAlarm - nFailed) * 0.1;
			qDebug() << endl << "False alarm = " << nFalseAlarm << ",  Failed = " << nFailed << ",  Avg = " << dSum;

			for (int n = 0; n<nFalseAlarm; n++)
				qDebug() << "False alarm Vehicle No." << nFalseAlarmList[n];

			for (int n = 0; n<nFailed; n++)
				qDebug() << "Failed Vehicle No." << nFailedCaseList[n];

			double dStd = 0.0;
			double dAvg = dSum * 10;
			for (int n = 0; n<nNumGap; n++)
			{
				dStd += (nGapList[n] - dAvg) * (nGapList[n] - dAvg);
			}
			dStd = dStd / nNumGap;
			qDebug() << "Standard Deviation = " << dStd;
		}
	}
#endif
	return false;
}

double CMySvm::bayesianFilter(int nTime, double dResult, double* dProb)
{
	if (nTime == 0)
	{
		for (int i = 0; i<NUM_CLASS; i++)
			m_dProbabiliy[i][nTime] = dProb[i];
	}
	else
	{
		for (int i = 0; i<NUM_CLASS; i++)
		{
			double dSum = 0.0;
			for (int j = 0; j<NUM_CLASS; j++)
			{
				dSum += CDatabase::GetInstance()->GetStateTransitionMatrix(j, i) * m_dProbabiliy[j][nTime - 1];
			}
			m_dProbabiliy[i][nTime] = dProb[i] * dSum;
		}
	}

	double dProbMax = DBL_MIN;
	for (int i = 0; i < NUM_CLASS; i++)
	{
		if (m_dProbabiliy[i][nTime] > dProbMax)
		{
			dProbMax = m_dProbabiliy[i][nTime];
			dResult = i + 1;
		}
	}

	return dResult;
}

void CMySvm::saveResult(void)
{
#if 0
	QString path = "../../Log/Result/";

	int nDataIndex = 0;
	int nTime = 0;

	QFile* fp;
	QTextStream* out;

	for (std::list <stFeatureVector>::iterator q = m_pt_list.begin(); q != m_pt_list.end(); q++)
	{
		int nVehicleNo = CDatabase::GetInstance()->GetDataInfo(nDataIndex, DATA_INFO_PACKET_VEHICLE_NO);
		int nDataLength = CDatabase::GetInstance()->GetDataInfo(nDataIndex, DATA_INFO_PACKET_DATA_LENGTH);

		if (nTime == 0)
		{
			QString file = path + QString::number(nVehicleNo) + ".csv";

			fp = new QFile(file);
			if (!fp->open(QIODevice::WriteOnly))
			{
				delete fp;
				return;
			}

			out = new QTextStream(fp);
		}

		*out << nVehicleNo << "," << nTime << ",";
		for (int k = 0; k<FEATURE_VECTOR_DIMENSION; k++)
			*out << q->x[k] << ",";
		*out << q->label << "," << q->prob << endl;

		nTime++;

		if (nTime == nDataLength)
		{
			nDataIndex++;
			nTime = 0;

			fp->close();

			delete fp;
			delete out;
		}
	}
#endif
}

void CMySvm::saveModel(svm_model* model)
{
#if 0
	time_t now = time(NULL);
	struct tm *pNow = localtime(&now);
	int nYear = pNow->tm_year + 1900;
	int nMonth = pNow->tm_mon + 1;
	int nDay = pNow->tm_mday;
	int nHour = pNow->tm_hour;
	int nMin = pNow->tm_min;
#endif

	QString modelFile = "Model/Model_K" + QString::number(FEATURE_VECTOR_DIMENSION) + "_C" + QString::number(m_nParamCost) + ".txt";
	std::string cStr = modelFile.toLocal8Bit();
	int nLen = cStr.length();
	char* chr = new char[nLen + 1];
	memcpy(chr, cStr.c_str(), nLen + 1);

	if (svm_save_model(chr, m_pModel) == 0)
	{
		qDebug() << "mysvm.cpp @ trained model was successfully saved";
	}
	else
	{
		qDebug() << "mysvm.cpp @ It was failed to save the trained model";
	}
}

void CMySvm::loadModel(svm_model* pModel)
{
	QString modelFile = "Model/Proposed(PF).txt";

	std::string cStr = modelFile.toLocal8Bit();
	int nLen = cStr.length();
	char* chr = new char[nLen + 1];
	memcpy(chr, cStr.c_str(), nLen + 1);

	m_pModel = svm_load_model(chr);

	delete[] chr;
}