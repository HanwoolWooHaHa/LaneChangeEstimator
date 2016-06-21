/**
* @file	vonMises.cpp
* @version	1.00
* @author	Hanwool Woo
* @date	Creation date: 2016/02/09
* @brief	this file is for the calculation of von Mises distribution
*/
#include "vonMises.h"
#include "bessel.h"

#include <qmath.h>
#include <qdebug.h>
/*********************************************************************/
CVonMises::CVonMises()
{
}

CVonMises::~CVonMises()
{
}
/*********************************************************************/
double CVonMises::Distribution( double dVelocity, double dAngle ) // unit : m/s, deg
{
	double dProb = 0.0;

	// Bessel関数に用いられる移動方向に関する平均方向母数，相対速度に対して値を決める
	double k = qAbs( dVelocity );
	if( k > 5.0 ) k = 5.0;

	double dTheta = 0.0;
	if( dVelocity >= 0.0 ) dTheta = 0.0;
	else dTheta = M_PI;

	// 第１種変形ベッセル関数
	double I = CBessel::GetInstance()->I0( k );

	dAngle = qDegreesToRadians( dAngle );

	if( (2 * M_PI * I) == 0.0 )
	{
		qDebug() << "vonMises.cpp @ the bessel function's value is zero";
		return -1;
	}

	// フォン・ミーゼス分布における値を計算する
	dProb = qExp( k * qCos( dAngle - dTheta ) ) / ( 2 * M_PI * I );

	return dProb;
}