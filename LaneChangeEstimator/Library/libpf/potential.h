#pragma once

/**
* @class	CPotential
* @author	Hanwool Woo
* @version	1.10
* @date	Creation date: 2016/02/09 \n
* 			    Last revision date: 2016/02/09 HanwoolWoo
* @brief	this class is for the generation of potential field
*/
class CPotential
{
public:
	static CPotential* GetInstance()
    {
        static CPotential* instance = new CPotential();
        return instance;
    }

    ~CPotential();

	/**
	* @fn Field
	* @param double dDistance : 相対距離 m, double dVelocity : 相対速度 m/s, double dAngle : 相対角度 deg
	* @return -1 : error, ベッセル関数の値が0である, otherwise : フォンミーゼス分布に従う確率
	* @remark this function is to calculate the distribution probability in von Mises distribution
	*/
	double Field( double dDistance, double dVelocity, int nDir );
	double Field( double dDistance, double dVelocity, double dAngle );

private:
	CPotential();


	double COEFFICIENT;
};