#pragma once

#define EARTH_RADIOUS 6378.137

class CSensor
{
public:
	static CSensor* GetInstance()
    {
        static CSensor* instance = new CSensor();
        return instance;
    }

	~CSensor();
	
	void SetOrigin( double dLongitude, double dLatitude );
	void CalcGlobalPosition( double dLongitude, double dLatitude, double* pdPosX, double* pdPosY );
	void CalcGlobalPosition( double dOwnLongitude, double dOwnLatitude, double dOwnAzimuth, double* pdPosX, double* pdPosY  );

	double CalculateAzimuth(double dLatA, double dLongA, double dLatB, double dLongB);
	double CalculateAttitude(double dPrePosX, double dPrePosY, double dPosX, double dPosY, int nCurrentLane, int nLaneMax, void* pdLaneData);

private:
	CSensor();

	double m_dOriginLongitude;
	double m_dOriginLatitude;
};