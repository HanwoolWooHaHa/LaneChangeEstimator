#pragma once

class CBessel
{
public:
	static CBessel* GetInstance()
    {
        static CBessel* instance = new CBessel();
        return instance;
    }

    ~CBessel();

	double I0(double x);

private:
	CBessel();
};