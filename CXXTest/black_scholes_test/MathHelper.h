#pragma once
#include <math.h>

namespace Mathhelper
{
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif // !M_PI


	//标准正太分布累积密度函数
	double nor_cdf(double d);
	

	//标准正太分布概率密度函数
	double nor_pdf(double d);

}