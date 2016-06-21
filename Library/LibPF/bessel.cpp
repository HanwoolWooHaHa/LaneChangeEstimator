#include "bessel.h"

#include <qdebug.h>
#include <qmath.h>

CBessel::CBessel()
{
}

CBessel::~CBessel()
{
}

double CBessel::I0(double x)
{
	int i;
	double w, wx375;
	static double a[6] = {	 1.0,        3.5156229,  3.0899424,
							 1.2067492,  0.2659732,  0.0360768};
	static double b[8] = {	 0.39894228,   0.013285917,  0.002253187,
							-0.001575649,  0.009162808, -0.020577063,
							 0.026355372, -0.016476329};

	if(x < 0.)
	{
		qDebug() << "Error : x < 0  in besi0()\n";
		return 0.;
	}
	if(x <= 3.75)
	{
		wx375 = x * x / 14.0625;
		w = 0.0045813;
		for(i = 5; i >= 0; i--)	w = w * wx375 + a[i];
		return w;
	}
	wx375 = 3.75 / x;
	w = 0.003923767;
	for(i = 7; i >= 0; i--)	w = w * wx375 + b[i];
	return w / qSqrt(x) * qExp(x);
}