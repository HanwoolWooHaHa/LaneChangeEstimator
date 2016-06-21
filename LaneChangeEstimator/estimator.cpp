/**
* @file	estimator.cpp
* @version	1.00
* @author	Hanwool Woo
* @date	Creation date: 2016/01/16
* @brief	this file is for the management of data files
*/
#include "estimator.h"
#include "mysvm.h"
#include "myHmm.h"
#include "defines.h"

#include <qdebug.h>

CEstimator::CEstimator( int nMethod )
{
	switch( nMethod )
	{
	case SVM:
		m_pMethod = new CMySvm();
		break;

	case HMM:
		m_pMethod = new CMyHmm();
		break;
	}
	
}

CEstimator::~CEstimator()
{
	if( m_pMethod != NULL )
		delete m_pMethod;
}
///////////////////////////////////////////////////////////////////////////
/* Public functions */
///////////////////////////////////////////////////////////////////////////
void CEstimator::Train( int nType )
{
	m_pMethod->Train( nType );
}

void CEstimator::Test( void )
{
	m_pMethod->Test();
}