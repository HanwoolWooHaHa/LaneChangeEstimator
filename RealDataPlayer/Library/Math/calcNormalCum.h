#pragma once

class CAdaptiveGL;

class CNormalCum
{
public:
	static CNormalCum* GetInstance()
    {
        static CNormalCum* instance = new CNormalCum();
        return instance;
    }

    ~CNormalCum();

	double CumNormal( double z );

private:
	CNormalCum();

	CAdaptiveGL* m_pAgl;
};