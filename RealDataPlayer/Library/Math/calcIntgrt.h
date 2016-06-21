#pragma once

class CAdaptiveGL
{
public:
	double (*f)(double);
	CAdaptiveGL() {};
	virtual ~CAdaptiveGL() {};

	double Integral( double a, double b, double (*func)(double) );
	void CalcIntegral( double& s0, double a, double b );
	double Gauss_Legendre( double a, double b );
};