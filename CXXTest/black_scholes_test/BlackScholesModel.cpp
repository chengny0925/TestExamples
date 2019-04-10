#include "BlackScholesModel.h"
#include "MathHelper.h"


BlackScholesModel::BlackScholesModel()
{
}


BlackScholesModel::~BlackScholesModel()
{
}

//S：标的资产现价
//X：执行价
//r：无风险利率
//q：连续分红率，Cost of Carry = r-q
//sigma：波动率
//t：距离到期时间
//type：Call/Put
double BlackScholesModel::GetOptionValue(double S, double X, double q, double r, double sigma, double t, OptionType type)
{
	double t_sqrt = sqrt(t);
	double sigma2 = sigma*sigma;
	double d1 = (log(S / X) + (r - q + sigma2*0.5)*t) / (t_sqrt*sigma);
	double d2 = d1 - t_sqrt*sigma;

	switch (type)
	{
	case OptionType::CALL:
		return S*exp(-q*t)*Mathhelper::nor_cdf(d1) - X*exp(-r*t)*Mathhelper::nor_cdf(d2);
	case OptionType::PUT:
		return -S*exp(-q*t)*Mathhelper::nor_cdf(-d1) + X*exp(-r*t)*Mathhelper::nor_cdf(-d2);
	default:
		return 0.0;
	}
}

double BlackScholesModel::GetDelta(double S, double X, double q, double r, double sigma, double t, OptionType type)
{
	double t_sqrt = sqrt(t);
	double sigma2 = sigma*sigma;
	double d1 = (log(S / X) + (r - q + sigma2*0.5)*t) / (t_sqrt*sigma);

	switch (type)
	{
	case OptionType::CALL:
		return exp(-q*t)*Mathhelper::nor_cdf(d1);
	case OptionType::PUT:
		return -exp(-q*t)*Mathhelper::nor_cdf(-d1);
	default:
		return 0.0;
	}
}

double BlackScholesModel::GetGamma(double S, double X, double q, double r, double sigma, double t, OptionType type)
{
	double t_sqrt = sqrt(t);
	double sigma2 = sigma*sigma;
	double d1 = (log(S / X) + (r - q + sigma2*0.5)*t) / (t_sqrt*sigma);

	return exp(-q*t)*Mathhelper::nor_pdf(d1) / S / t_sqrt / sigma;
}

double BlackScholesModel::GetTheta(double S, double X, double q, double r, double sigma, double t, OptionType type)
{
	double t_sqrt = sqrt(t);
	double sigma2 = sigma*sigma;
	double d1 = (log(S / X) + (r - q + sigma2*0.5)*t) / (t_sqrt*sigma);
	double d2 = d1 - t_sqrt*sigma;

	double part1 = S*sigma*exp(-q*t)*Mathhelper::nor_pdf(d1) / 2.0 / t_sqrt;
	double part2 = -q*S*exp(-q*t);
	double part3 = r*X*exp(-r*t);
	switch (type)
	{
	case OptionType::CALL:
		return -part1 - part2*Mathhelper::nor_cdf(d1) - part3*Mathhelper::nor_cdf(d2);
	case OptionType::PUT:
		return -part1 + part2*Mathhelper::nor_cdf(-d1) + part3*Mathhelper::nor_cdf(-d2);
	default:
		return 0.0;
	}
}

double BlackScholesModel::GetVega(double S, double X, double q, double r, double sigma, double t, OptionType type)
{
	double t_sqrt = sqrt(t);
	double sigma2 = sigma*sigma;
	double d1 = (log(S / X) + (r - q + sigma2*0.5)*t) / (t_sqrt*sigma);

	return S*exp(-q*t)*Mathhelper::nor_pdf(d1)*t_sqrt;
}

double BlackScholesModel::GetRho(double S, double X, double q, double r, double sigma, double t, OptionType type)
{
	double t_sqrt = sqrt(t);
	double sigma2 = sigma*sigma;
	double d1 = (log(S / X) + (r - q + sigma2*0.5)*t) / (t_sqrt*sigma);
	double d2 = d1 - t_sqrt*sigma;

	switch (type)
	{
	case OptionType::CALL:
		return t*X*exp(-r*t)*Mathhelper::nor_cdf(d2);
	case OptionType::PUT:
		return -t*X*exp(-r*t)*Mathhelper::nor_cdf(-d2);
	default:
		return 0.0;
	}
}

double BlackScholesModel::GetImpliedVol(double S, double X, double q, double r, double optionPrice, double t, OptionType type, double accuracy, int maxIterations)
{
	if (optionPrice<0.99*(S - X*exp(-r*t)))
		return 0.0;
	double t_sqrt = sqrt(t);
	double sigma = optionPrice / S / 0.398 / t_sqrt;
	for (int i = 0; i<maxIterations; i++)
	{
		double price = GetOptionValue(S, X, q, r, sigma, t, type);
		double diff = optionPrice - price;
		if (fabs(diff)<accuracy)
			return sigma;
		double vega = GetVega(S, X, q, r, sigma, t, type);
		sigma = sigma + diff / vega;
	}
	return sigma;
}
