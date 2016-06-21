#pragma once
#include "../defines.h"

class CViterbi
{
public:
    static CViterbi* GetInstance()
    {
        static CViterbi* instance = new CViterbi();
        return instance;
    }

    ~CViterbi() {}

    int Estimate( int nIndex, int nTick, stHMM* pModel, double* pdLikelihood );
	int GetStateSeq( int nTick )
	{
		return m_nStateSeq[nTick];
	}
	void ResetStateSeq( void );

private:
    CViterbi() {}

	double m_dDelta[STATE_NUM][T_MAX];
	int m_nStateSeq[T_MAX];
};
