#ifndef FORWARDALG_H
#define FORWARDALG_H

#include "../../constant.h"

class forwardAlg
{
public:
    static forwardAlg* GetInstance()
    {
        static forwardAlg* instance = new forwardAlg();
        return instance;
    }

    ~forwardAlg() {}

    double GetProbability( int index, stHMM* model );

    float likelihood[T_MAX];
    float alpha[STATE_NUM][T_MAX];

private:
    forwardAlg() {}

    void resetLikelihoodData( void );
};

#endif // FORWARDALG_H
