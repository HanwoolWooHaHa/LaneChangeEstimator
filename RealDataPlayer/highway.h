#pragma once

class CHighway
{
public:
    static CHighway* GetInstance()
	{
        static CHighway* ret = new CHighway();

		return ret;
	}

	double dCube[8][3];

private:
    CHighway()
	{
		dCube[0][0] = -7000.0;
        dCube[0][1] = 0.0; //-LANE_WIDTH * FEET_TO_METER;
		dCube[0][2] = -0.3;

		dCube[1][0] = -7000.0;
        dCube[1][1] = 4500;
		dCube[1][2] = -0.3;

		dCube[2][0] = -7000.0;
        dCube[2][1] = 4500;
		dCube[2][2] = 0.0;

		dCube[3][0] = -7000.0;
        dCube[3][1] = 0.0; //-LANE_WIDTH * FEET_TO_METER;
		dCube[3][2] = 0.0;

		dCube[4][0] = 0.0;
        dCube[4][1] = 0.0; //-LANE_WIDTH * FEET_TO_METER;
		dCube[4][2] = -0.3;

		dCube[5][0] = 0.0;
        dCube[5][1] = 4500;
		dCube[5][2] = -0.3;

		dCube[6][0] = 0.0;
        dCube[6][1] = 4500;
		dCube[6][2] = 0.0;

		dCube[7][0] = 0.0;
        dCube[7][1] = 0.0; //-LANE_WIDTH * FEET_TO_METER;
		dCube[7][2] = 0.0;
	}

    ~CHighway() {}
};
