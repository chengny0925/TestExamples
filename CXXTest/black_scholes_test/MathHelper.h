#pragma once
#include <math.h>

namespace Mathhelper
{
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif // !M_PI


	//��׼��̫�ֲ��ۻ��ܶȺ���
	double nor_cdf(double d);
	

	//��׼��̫�ֲ������ܶȺ���
	double nor_pdf(double d);

}