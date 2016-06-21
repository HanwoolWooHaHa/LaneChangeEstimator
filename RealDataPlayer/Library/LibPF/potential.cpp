/**
* @file	potential.cpp
* @version	1.00
* @author	Hanwool Woo
* @date	Creation date: 2016/02/09
* @brief	this file is for the generation of potential field
*/
#include "potential.h"
#include "vonMises.h"

#include <qmath.h>
/*********************************************************************/
CPotential::CPotential()
{
	COEFFICIENT = 1.0;
}

CPotential::~CPotential()
{
}
/*********************************************************************/
double CPotential::Field( double dDistance, double dVelocity, int nDir )
{
	double dProb = 0.0;
	double dAngle = 0.0;

	if( nDir == 0 )
	{
		if( dVelocity >= 0.0 )
			dAngle = 180.0;
		else
			dAngle = 0.0;
	}
	else
	{
		if( dVelocity >= 0.0 )
			dAngle = 0.0;
		else
			dAngle = 180.0;
	}

	dVelocity = qAbs( dVelocity );

	// 周辺車両との相対量に対応するポテンシャル場を生成する
	dProb = qExp( -dDistance * 0.05 ) * CVonMises::GetInstance()->Distribution( dVelocity, dAngle ) * COEFFICIENT;

	return dProb;
}

double CPotential::Field( double dDistance, double dVelocity, double dAngle )
{
	double dProb = 0.0;

	// 周辺車両との相対量に対応するポテンシャル場を生成する
	dProb = qExp( -dDistance * 0.05 ) * CVonMises::GetInstance()->Distribution( dVelocity, dAngle ) * COEFFICIENT;

	return dProb;
}