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
	* @param double dVelocity : ���Α��x m/s, double dAngle : ���Ίp�x deg
	* @return -1 : error, �x�b�Z���֐��̒l��0�ł���, otherwise : �t�H���~�[�[�X���z�ɏ]���m��
	* @remark this function is to calculate the distribution probability in von Mises distribution
	*/
	double Distribution( double dVelocity, double dAngle );

private:
	CVonMises();
};