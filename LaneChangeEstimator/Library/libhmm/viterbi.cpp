#include "viterbi.h"
#include "../database.h"
#include "emission.h"

#include <qmath.h>
#include <qdebug.h>

int CViterbi::Estimate( int nIndex, int nTick, stHMM* pModel, double* pdLikelihood )
{
	int nState = 0;
	void* pData = CDatabase::GetInstance()->GetDataPointer( FEATURE );

	memset( m_dDelta, 0, sizeof( double ) * STATE_NUM * T_MAX );
	memset( m_nStateSeq, 0, sizeof( int ) * T_MAX );

	for( int i=0; i<STATE_NUM; i++ )
    {
        m_dDelta[i][0] = pModel->INIT[i];
        m_dDelta[i][0] = qLn( m_dDelta[i][0] );
    }

	if( nTick == 0 )
    {
        double dMax = -DBL_MAX;
        for( int i=0; i<STATE_NUM; i++ )
        {
            if( m_dDelta[i][0] > dMax )
            {
                dMax = m_dDelta[i][0];
                nState = i;
            }
        }

        m_nStateSeq[nTick] = nState;
        *pdLikelihood = dMax;

        return nState+1;
    }

	int nDataLength = CDatabase::GetInstance()->GetDataInfo( nIndex, DATA_INFO_PACKET_DATA_LENGTH );
	if( nTick >= nDataLength )
	{
        qDebug() << "viterbi.cpp @ time is over the Tmax";
        return -1;
    }

	for( int t=1; t<nTick; t++ )
	{
		for( int i=0; i<STATE_NUM; i++ )
		{
			if( t==120 )
				t=t;

			double dMax = -DBL_MAX;
            int nMax_j = 0;
            double dValue;

			for( int j=0; j<STATE_NUM; j++ )
            {
				dValue = m_dDelta[j][t-1] + qLn( pModel->TRANS[j][i] );
                if( dValue > dMax )
                {
                    nMax_j = j;
                    dMax = dValue;
                }
            }

			double dEmis = qLn( CEmission::GetInstance()->GetEmisProbability( i, t, pModel, pData, nIndex ) );

			m_dDelta[i][t] = dMax + dEmis;
		}
	}

	double dMax = -DBL_MAX;
    int nMax_j = 0;

    for( int j=0; j<STATE_NUM; j++ )
    {
        double dValue = m_dDelta[j][nTick-1];

        if( dValue > dMax )
        {
            dMax = dValue;
            nMax_j = j;
        }
    }

	nState = nMax_j;
    *pdLikelihood = dMax;

	dMax = -DBL_MAX;
    nMax_j = 0;

	for( int t=nTick-1; t>=0; t-- )
	{
		for( int j=0; j<STATE_NUM; j++ )
		{
			double dValue = m_dDelta[j][t];

			 if( dValue > dMax )
			{
				dMax = dValue;
				nMax_j = j;
			}
		}

		m_nStateSeq[t] = nMax_j+1;
	}

	return nState + 1;
}

void CViterbi::ResetStateSeq( void )
{
	memset( m_nStateSeq, 0, sizeof( int ) * T_MAX );
}