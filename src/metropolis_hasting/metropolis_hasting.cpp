#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <fstream>
#include <numbers>


const unsigned ITERATIONS = 1000;
const double MAX_STEP = 0.01;
const double X_INITIAL = 0.5;

double p(double x, double mean = 0.5, double stddev = 0.01) {
	double variance = stddev * stddev;
	return (1 / std::sqrt(2 * std::numbers::pi * variance)) * std::exp(-(std::pow((x - mean), 2) / (2 * variance)));
}

int main()
{
	std::ofstream out("output.txt");
	if (!out) {
		std::cerr << "Unable to write output.txt.." << std::endl;
		return -1;
	}

	double x_pre = X_INITIAL, x_next = X_INITIAL;
	for (unsigned i = 0; i != ITERATIONS; ++i) {
		std::random_device rd;
		std::default_random_engine generator(rd());
		std::uniform_real_distribution<double> u(-MAX_STEP, MAX_STEP);
		x_next += u(rd);

		double p_accept = std::min((double)1, p(x_next) / p(x_pre));
		std::uniform_real_distribution<double> random(0, 1);
		if (random(rd) < p_accept) {
			out << x_next << ' ';
			x_pre = x_next;
		}
		else {
			x_next = x_pre;
		}
	}

	out << std::endl;
}
