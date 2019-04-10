#pragma once

enum class OptionType
{
	CALL=0,
	PUT
};

class BlackScholesModel
{
public:
	BlackScholesModel();
	~BlackScholesModel();

public:
	double GetOptionValue(double S, double X, double q, double r,
		double sigma, double t, OptionType type);
	double GetDelta(double S, double X, double q, double r,
		double sigma, double t, OptionType type);
	double GetGamma(double S, double X, double q, double r,
		double sigma, double t, OptionType type);
	double GetTheta(double S, double X, double q, double r,
		double sigma, double t, OptionType type);
	double GetVega(double S, double X, double q, double r,
		double sigma, double t, OptionType type);
	double GetRho(double S, double X, double q, double r,
		double sigma, double t, OptionType type);

	double GetImpliedVol(double S, double X, double q, double r, double optionPrice,
		double t, OptionType type, double accuracy = 1e-6, int maxIterations = 100);
};

