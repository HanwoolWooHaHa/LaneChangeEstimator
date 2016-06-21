#pragma once

struct stHMM;

class CEmission
{
public:
    static CEmission* GetInstance()
    {
        static CEmission* instance = new CEmission();
        return instance;
    }

    ~CEmission();

    double GetEmisProbability( int nState, int nTick, stHMM* model, void* pData, int nIndex );

private:
	CEmission() {}

	double gaussianProbabilityDistribution( double dValue, double dAvg, double dStd );
};
