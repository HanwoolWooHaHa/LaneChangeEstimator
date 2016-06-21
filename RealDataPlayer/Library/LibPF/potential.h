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
	* @param double dDistance : ���΋��� m, double dVelocity : ���Α��x m/s, double dAngle : ���Ίp�x deg
	* @return -1 : error, �x�b�Z���֐��̒l��0�ł���, otherwise : �t�H���~�[�[�X���z�ɏ]���m��
	* @remark this function is to calculate the distribution probability in von Mises distribution
	*/
	double Field( double dDistance, double dVelocity, int nDir );
	double Field( double dDistance, double dVelocity, double dAngle );

private:
	CPotential();


	double COEFFICIENT;
};