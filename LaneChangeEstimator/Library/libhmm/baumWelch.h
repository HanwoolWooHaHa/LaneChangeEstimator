#pragma once

#include "../defines.h"

class CBaumWelch
{
public:
	CBaumWelch();
	~CBaumWelch();

    int Train( stHMM* model );

private:
    float gamma_tij[NUM_TRAFFIC_DATA][T_MAX][STATE_NUM][STATE_NUM];
    float gamma_ti[NUM_TRAFFIC_DATA][T_MAX][STATE_NUM];
    float m_likelihood[NUM_TRAFFIC_DATA];
    float m_alpha[NUM_TRAFFIC_DATA][STATE_NUM][T_MAX];
    float m_beta[NUM_TRAFFIC_DATA][STATE_NUM][T_MAX];
};
