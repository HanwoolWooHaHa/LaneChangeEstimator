#ifndef BACKWARDALG_H
#define BACKWARDALG_H

#include "../defines.h"

class backwardAlg
{
public:
    static backwardAlg* GetInstance()
    {
        static backwardAlg* instance = new backwardAlg();
        return instance;
    }

    ~backwardAlg() {}

    double GetProbability( int index, stHMM *model );

    float beta[STATE_NUM][T_MAX];

private:
    backwardAlg() {}

    void resetLikelihoodData( void );
};

#endif // BACKWARDALG_H
