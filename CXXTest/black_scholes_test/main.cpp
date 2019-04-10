#include "MathHelper.h"
#include <iostream>
#include "BlackScholesModel.h"

using namespace std;

void test()
{
	double S = 164, X = 165, r = 0.054, sigma = sqrt(0.0841), t = 0.0959;
	BlackScholesModel model;
	{
		double call_price = model.GetOptionValue(S, X, 0, r, sigma, t, OptionType::CALL);
		double delta = model.GetDelta(S, X, 0, r, sigma, t, OptionType::CALL);
		double gamma = model.GetGamma(S, X, 0, r, sigma, t, OptionType::CALL);
		double theta = model.GetTheta(S, X, 0, r, sigma, t, OptionType::CALL);
		double vega = model.GetVega(S, X, 0, r, sigma, t, OptionType::CALL);
		double rho = model.GetRho(S, X, 0, r, sigma, t, OptionType::CALL);

		std::cout << "call theory price:" << call_price << endl;
		std::cout << "call delta:" << delta << endl;
		std::cout << "call gamma:" << gamma << endl;
		std::cout << "call theta:" << theta << endl;
		std::cout << "call vega:" << vega << endl;
		std::cout << "call rho:" << rho << endl;
	}

	{
		double put_price = model.GetOptionValue(S, X, 0, r, sigma, t, OptionType::PUT);
		double delta = model.GetDelta(S, X, 0, r, sigma, t, OptionType::PUT);
		double gamma = model.GetGamma(S, X, 0, r, sigma, t, OptionType::PUT);
		double theta = model.GetTheta(S, X, 0, r, sigma, t, OptionType::PUT);
		double vega = model.GetVega(S, X, 0, r, sigma, t, OptionType::PUT);
		double rho = model.GetRho(S, X, 0, r, sigma, t, OptionType::PUT);

		std::cout << "put theory price:" << put_price << endl;
		std::cout << "put delta:" << delta << endl;
		std::cout << "put gamma:" << gamma << endl;
		std::cout << "put theta:" << theta << endl;
		std::cout << "put vega:" << vega << endl;
		std::cout << "put rho:" << rho << endl;
	}
}

int main()
{
	double nd1 = Mathhelper::nor_cdf(0.03);
	double nd2 = Mathhelper::nor_cdf(-0.06);
	cout << "nor_cdf(0.03)=" << nd1 << endl;
	cout << "nor_cdf(-0.06)=" << nd2 << endl;

	
	test();
	double S = 164, X = 165, r = 0.054, sigma = sqrt(0.0841), t = 0.0959;
	BlackScholesModel model;
	double real_price = 5.2;
	double impvol = model.GetImpliedVol(S, X, 0, r, real_price, t, OptionType::CALL, 1e-6, 100);
	cout << "impvol:" << impvol << endl;


	return 0;
}