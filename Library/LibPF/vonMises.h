#pragma once

/**
* @class	CVonMises
* @author	Hanwool Woo
* @version	1.10
* @date	Creation date: 2016/02/09 \n
* 			    Last revision date: 2016/02/09 HanwoolWoo
* @brief	this class is for the calculation of von Mises distribution
*/
class CVonMises
{
public:
	static CVonMises* GetInstance()
    {
        static CVonMises* instance = new CVonMises();
        return instance;
    }

    ~CVonMises();

	/**
	* @fn Distribution
	* @param double dVelocity : 相対速度 m/s, double dAngle : 相対角度 deg
	* @return -1 : error, ベッセル関数の値が0である, otherwise : フォンミーゼス分布に従う確率
	* @remark this function is to calculate the distribution probability in von Mises distribution
	*/
	double Distribution( double dVelocity, double dAngle );

private:
	CVonMises();
};