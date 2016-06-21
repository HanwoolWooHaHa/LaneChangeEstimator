#pragma once

class CMethod
{
public:
	virtual void Train(int nType) = 0;
	virtual bool Test(int nTick, int nTargetIndex) = 0;
};