#include <QString>
#include <QDebug>
#include <float.h>

#include "backwardAlg.h"
#include "forwardAlg.h"
#include "database.h"
#include "emission.h"

double backwardAlg::GetProbability( int nIndex, stHMM *model )
{
    CDatabase* data = CDatabase::GetInstance();
    forwardAlg* forward = forwardAlg::GetInstance();

    double prob = 0.0;

    //int vehicleNo = data->GetTrainingDataInfo( index, 0 );
    int Tmax = data->GetDataInfo( nIndex, DATA_INFO_PACKET_DATA_LENGTH );
	void* pData = data->GetDataPointer( FEATURE );

    resetLikelihoodData();

    int t = Tmax - 1;
    for( int i=0; i<STATE_NUM; i++ )
    {
        beta[i][t] = forward->likelihood[t];
    }

    for( t=Tmax-2; t>=0; t-- )
    {
        for( int i=0; i<STATE_NUM; i++ )
        {
            prob = 0.0;
            for( int j=0; j<STATE_NUM; j++ )
            {
                prob += model->TRANS[i][j] * CEmission::GetInstance()->GetEmisProbability( j, t+1, model, pData, nIndex ) * beta[j][t+1];

                if( prob > DBL_MAX)
                {
                    qDebug() << "beta was over! No." << data->GetDataInfo( nIndex, DATA_INFO_PACKET_VEHICLE_NO );

                    return -1.0;
                }
            }

            beta[i][t] = prob * forward->likelihood[t];
        }
    }

    t = 0;
    prob = 0.0;

    for( int i=0; i<STATE_NUM; i++ )
    {
        prob += model->INIT[i] * CEmission::GetInstance()->GetEmisProbability( i, t, model, pData, nIndex ) * beta[i][t];
    }

    return prob;
}

void backwardAlg::resetLikelihoodData( void )
{
    memset( beta, 0, sizeof(float) * STATE_NUM * T_MAX );
}
