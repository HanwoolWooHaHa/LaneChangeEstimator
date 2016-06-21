#include "forwardAlg.h"
#include "database.h"
#include "emission.h"

#include <QString>
#include <QDebug>

double forwardAlg::GetProbability( int nIndex, stHMM *model )
{
    CDatabase* data = CDatabase::GetInstance();

    double prob = 0.0;

	int Tmax = data->GetDataInfo( nIndex, DATA_INFO_PACKET_DATA_LENGTH );
	void* pData = data->GetDataPointer( FEATURE );

    resetLikelihoodData();

    for( int i=0; i<STATE_NUM; i++ )
    {
        alpha[i][0] = model->INIT[i] * CEmission::GetInstance()->GetEmisProbability( i, 0, model, pData, nIndex );
        likelihood[0] += alpha[i][0];
    }

    if( likelihood[0] == 0.0 )
    {
        qDebug() << "likelihood error time = 0";
        return -1;
    }

    likelihood[0] = 1 / likelihood[0];

    float check = 0.0;
    for( int i=0; i<STATE_NUM; i++ )
    {
        alpha[i][0] *= likelihood[0];
        check += alpha[i][0];
    }

    //qDebug() << "likelihood 0 = " << check;

    for( int t=0; t<Tmax-1; t++ )
    {
        for( int j=0; j<STATE_NUM; j++ )
        {
            prob = 0.0;
            for( int i=0; i<STATE_NUM; i++ )
            {
                prob += alpha[i][t] * model->TRANS[i][j];
            }

            alpha[j][t+1] = prob * CEmission::GetInstance()->GetEmisProbability(  j, t+1, model, pData, nIndex );
            likelihood[t+1] += alpha[j][t+1];
        }

        if( likelihood[t+1] == 0.0 )
        {
            qDebug() << "likelihood error time = " << t+1;
            return -2;
        }

        likelihood[t+1] = 1 / likelihood[t+1];

        for( int j=0; j<STATE_NUM; j++ )
        {
            alpha[j][t+1] *= likelihood[t+1];
        }

        //qDebug() << "time : " << t+1 << ", " << likelihood[t+1];
    }

    int t = Tmax - 1;
    prob = 0.0;

    for( int i=0; i<STATE_NUM; i++ )
    {
        prob += alpha[i][t];
    }

    //qDebug() << "Vehicle No." << vehicleNo << ", likelihood : " << 1/likelihood[t];

    return prob;
}

void forwardAlg::resetLikelihoodData( void )
{
    memset( likelihood, 0, sizeof(float) * T_MAX );
    memset( alpha, 0, sizeof(float) * STATE_NUM * T_MAX );
}
