#include <string>
#include <math.h>
#include <float.h>
#include <QDebug>

#include "baumWelch.h"
#include "database.h"
#include "forwardAlg.h"
#include "backwardAlg.h"
#include "emission.h"


/********************************************************/
/************** public member functions **************/
/********************************************************/
CBaumWelch::CBaumWelch()
{
}

CBaumWelch::~CBaumWelch()
{
}

int CBaumWelch::Train( stHMM *model  )
{
    CDatabase* data = CDatabase::GetInstance();
    forwardAlg* forward = forwardAlg::GetInstance();
    backwardAlg* backward = backwardAlg::GetInstance();

	int dataNum = data->GetNumData();
	double (*pdFeatureData)[T_MAX][FEATURE_VECTOR_DIMENSION] = reinterpret_cast<double(*)[T_MAX][FEATURE_VECTOR_DIMENSION]>( data->GetDataPointer( FEATURE ) );

    memset( gamma_tij, 0, sizeof(float) * NUM_TRAFFIC_DATA * T_MAX * STATE_NUM * STATE_NUM );
    memset( gamma_ti, 0, sizeof(float) * NUM_TRAFFIC_DATA * T_MAX * STATE_NUM );
    memset( m_alpha, 0, sizeof(float) * NUM_TRAFFIC_DATA * STATE_NUM * T_MAX );
    memset( m_beta, 0, sizeof(float) * NUM_TRAFFIC_DATA * STATE_NUM * T_MAX );
	model->likelihood = 0.0;

    //! 1. calculate gamma
    for( int n=0; n<dataNum; n++ )
    {
        double prob = forward->GetProbability( n, model );
        double prob2 = backward->GetProbability( n, model );

        if( prob < 0 || prob2 < 0 )
        {
            qDebug() << "Forward or Backward Error";
            return -1;
        }

		int Tmax = data->GetDataInfo( n, DATA_INFO_PACKET_DATA_LENGTH );

        for( int t=0; t<Tmax-1; t++ )
        {
            for( int i=0; i<STATE_NUM; i++ )
            {
                gamma_ti[n][t][i] = 0.0;
                for( int j=0; j<STATE_NUM; j++ )
                {
                    gamma_tij[n][t][i][j] = forward->alpha[i][t] * model->TRANS[i][j] * CEmission::GetInstance()->GetEmisProbability( j, t+1, model, reinterpret_cast<void*>(pdFeatureData), n ) * backward->beta[j][t+1]; // * forward->likelihood[Tmax-1];
                    gamma_ti[n][t][i] += gamma_tij[n][t][i][j];
                }

                m_alpha[n][i][t] = forward->alpha[i][t];
                m_beta[n][i][t] = backward->beta[i][t];
            }
        }

        m_likelihood[n] = forward->likelihood[Tmax-1];

        model->likelihood += 1 / m_likelihood[n];
    }

    //! 2. re-estimate HMM parameter
    //! 2.1 re-estimate initial probability
    for( int i=0; i<STATE_NUM; i++ )
    {
        float sum = 0.0;
        for( int n=0; n<dataNum; n++ )
        {
            sum += gamma_ti[n][0][i];
        }

        model->INIT[i] = sum / dataNum;
    }

    //! re-estimate transition probability
    for( int i=0; i<STATE_NUM; i++ )
    {
        for( int j=0; j<STATE_NUM; j++ )
        {
            double sum_upper = 0.0;
            double sum_lower = 0.0;

            for( int n=0; n<dataNum; n++ )
            {
                double upper = 0.0;
                double lower = 0.0;

                int Tmax = data->GetDataInfo( n, DATA_INFO_PACKET_DATA_LENGTH );

                for( int t=0; t<Tmax-1; t++ )
                {
                    upper += gamma_tij[n][t][i][j];
                    lower += gamma_ti[n][t][i];
                }

                sum_upper += upper;
                sum_lower += lower;
            }

            model->TRANS[i][j] = sum_upper / sum_lower;
        }
    }

    //! 2.3 re-estimate emission parameter : average, standard deviation
    //! 2.3.1 re-estimate average
    float tempEMIS[STATE_NUM][FEATURE_VECTOR_DIMENSION][2];

    for( int i=0; i<STATE_NUM; i++ )
    {
		double sum_upper[FEATURE_VECTOR_DIMENSION] = { 0.0 };
        double sum_lower = 0.0;

        for( int n=0; n<dataNum; n++ )
        {
			double upper[FEATURE_VECTOR_DIMENSION] = { 0.0 };
            double lower = 0.0;

            int Tmax = data->GetDataInfo( n, DATA_INFO_PACKET_DATA_LENGTH );

            for( int t=0; t<Tmax-1; t++ )
            {
				for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
					upper[k] += m_alpha[n][i][t] * m_beta[n][i][t] * pdFeatureData[n][t][k];
                
				lower += m_alpha[n][i][t] * m_beta[n][i][t];
            }

			for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
				sum_upper[k] += upper[k];

            sum_lower += lower;
        }

		for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
			tempEMIS[i][k][0] = sum_upper[k] / sum_lower;
    }

    //! 2.3.2 re-estimate standard deviation
    for( int i=0; i<STATE_NUM; i++ )
    {
		double sum_upper[FEATURE_VECTOR_DIMENSION] = { 0.0 };
        double sum_lower = 0.0;

        for( int n=0; n<dataNum; n++ )
        {
			double upper[FEATURE_VECTOR_DIMENSION] = { 0.0 };
            double lower = 0.0;

            int Tmax = data->GetDataInfo( n, DATA_INFO_PACKET_DATA_LENGTH );

            for( int t=0; t<Tmax-1; t++ )
            {
                for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
				{
					float value = pdFeatureData[n][t][k];
					float avg = model->EMIS[i][k][0];

					upper[k] += m_alpha[n][i][t] * m_beta[n][i][t] * ( value - avg ) * ( value - avg );
				}

                lower += m_alpha[n][i][t] * m_beta[n][i][t];
            }

			for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
				sum_upper[k] += upper[k];

            sum_lower += lower;
        }

		for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
			tempEMIS[i][k][1] = sum_upper[k] / sum_lower;
    }

    for( int i=0; i<STATE_NUM; i++ )
    {
		for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
		{
			model->EMIS[i][k][0] = tempEMIS[i][k][0];
			model->EMIS[i][k][1] = tempEMIS[i][k][1];
		}
    }

    return 0;
}
