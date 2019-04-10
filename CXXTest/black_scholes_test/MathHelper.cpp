#include "MathHelper.h"


//标准正太分布累积密度函数
double Mathhelper::nor_cdf(double d)
{
	/*
	//Bagby, R. J. "Calculating Normal Probabilities." Amer. Math. Monthly 102, 46-49, 1995
	double part1 = 7.0*Math.Exp(-d*d/2.0);
	double part2 = 16.0*Math.Exp(-d*d*(2.0-Math.Sqrt(2.0)));
	double part3 = (7.0+Math.PI*d*d/4.0)*Math.Exp(-d*d);
	double cumProb = Math.Sqrt(1.0-(part1+part2+part3)/30.0)/2.0;
	if(d>0)
	cumProb = 0.5+cumProb;
	else
	cumProb = 0.5-cumProb;
	return cumProb;
	*/

	//出自Financial Numerical Recipes in C++
	if (d > 6.0)
		return 1.0;
	else if (d < -6.0)
		return 0.0;
	double b1 = 0.31938153;
	double b2 = -0.356563782;
	double b3 = 1.781477937;
	double b4 = -1.821255978;
	double b5 = 1.330274429;
	double p = 0.2316419;
	double c2 = 0.3989423;

	double a = fabs(d);
	double t = 1.0 / (1.0 + a*p);
	double b = c2*exp((-d)*(d*0.5));
	double n = ((((b5*t + b4)*t + b3)*t + b2)*t + b1)*t;
	n = 1.0 - b*n;
	if (d < 0)
		n = 1.0 - n;
	return n;
}

//标准正太分布概率密度函数
double Mathhelper::nor_pdf(double d)
{
	return 1.0 / sqrt(2.0*M_PI)*exp(-d*d*0.5);
}
