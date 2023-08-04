#include <iostream>
#include <vector>
#include <random>
#include "../tools/data_processing.h"
#include "../../deps/Xoshiro-cpp/XoshiroCpp.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>

using matools::load_vector;

[[nodiscard]] inline static unsigned random(const unsigned a, const unsigned b) noexcept;
[[nodiscard]] inline static double random() noexcept;
[[nodiscard]] static unsigned z(std::vector<unsigned> route, std::vector<std::vector<unsigned>> mat) noexcept;

std::vector<std::vector<unsigned>> mat;
int traveller()
{
	mat = load_vector<unsigned>("data/matrix.txt");
	if (mat.empty()) return EXIT_FAILURE;
	std::ofstream output("saft_output.txt");
	if (!output) {
		std::cerr << "can't create file..." << std::endl;
		return EXIT_FAILURE;
	}
	std::vector<unsigned> route{1, 2, 3, 4, 5, 6, 7, 1};
	unsigned z_p = z(route, mat);
	double t = z_p;
	unsigned cnt = 0;

	while (cnt <= 10) {
		unsigned beg = random(1, 5);
		unsigned end = random(beg + 1, 6);

		std::vector<unsigned> temp_route(route);
		std::vector<unsigned>::iterator start = temp_route.begin();
		std::vector<unsigned>::iterator reverse_start = start + beg;
		std::vector<unsigned>::iterator reverse_end = start + end + 1;
		std::reverse(reverse_start, reverse_end);

		if (!mat[*reverse_start - 1][*(reverse_start - 1) - 1] ||
			!mat[*reverse_end - 1][*(reverse_end - 1) - 1])
			continue;


		unsigned z_now = z(temp_route, mat);
		double rdd = random();
		double e = std::exp(z_p - z_now) / t;
		if (z_now <= z_p) {
			z_p = z_now;
			route = temp_route;
			cnt = 0;
			t /= 2;
			output << beg << ' ' << end << " | ";
			for (unsigned num : temp_route) {
				output << num << ' ';
			}
			output << "__distance:" << z_now << ' ';
			output << "accepted!" << '\n';
		}
		else if (rdd < e && t > 1e-20) {
			z_p = z_now;
			route = temp_route;
			cnt = 0;
			output << "random accepted..." << '\n';
		}
		else {
			cnt++;
		}


	}

	output.close();
	return 0;
}

[[nodiscard]] inline static double random() noexcept
{

	auto seed = XoshiroCpp::DefaultSeed;
	XoshiroCpp::Xoroshiro128PlusPlus gen(seed);

	return XoshiroCpp::DoubleFromBits(gen());

}
[[nodiscard]] inline static unsigned random(const unsigned a, const unsigned b) noexcept
{
	auto seed = XoshiroCpp::DefaultSeed;
	XoshiroCpp::Xoshiro128Plus gen(seed);
	std::uniform_int_distribution<unsigned> dis(a, b);

	return dis(gen);
}

[[nodiscard]] static unsigned z(const std::vector<unsigned> route, const std::vector<std::vector<unsigned>> mat) noexcept
{
	unsigned total = 0;
	for (auto i = route.begin() + 1; i != route.end(); ++i) {
		unsigned start_point = *(i - 1) - 1;
		unsigned end_point = *(i)-1;
		assert(mat[start_point][end_point] > 0);
		total += mat[start_point][end_point];
	}

	return total;
}