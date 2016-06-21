#include "calcNormalCum.h"
#include "calcIntgrt.h"

#include <cmath>

CNormalCum::CNormalCum()
{
	m_pAgl = new CAdaptiveGL();
}

CNormalCum::~CNormalCum()
{
	delete m_pAgl;
}

double knormal( double z )
{
	return exp( -0.5 * z * z );
}

double CNormalCum::CumNormal( double z )
{
	double p = 0.0;

	double pi = atan(1.0) * 4.0;
	if (z == 0.0)
	{
		p = 0.5;
	}else if(z > 0.0)
	{
		p = 0.5 + m_pAgl->Integral( 0.0, z, knormal ) / sqrt(2.0 * pi);
	}
	else
	{
		p = 0.5 - m_pAgl->Integral( z, 0.0, knormal ) / sqrt(2.0 * pi);
	}
	
	return p;
}