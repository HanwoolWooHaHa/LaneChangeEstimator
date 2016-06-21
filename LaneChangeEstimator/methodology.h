#pragma once

class CMethodology
{
public:
	virtual void Train( int nType ) = 0;
	virtual void Test( void ) = 0;
};