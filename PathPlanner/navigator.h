#pragma once

class CNavigationMethod;

class CNavigator
{
public:
	CNavigator(double dDeltaTs);
	~CNavigator();

	void Initialize( void );
	void Update( int nTick );

private:
	CNavigationMethod* m_pNavigationMethod;
	CNavigationMethod* m_pAdjustMethod;

	double m_dDeltaTs;
};