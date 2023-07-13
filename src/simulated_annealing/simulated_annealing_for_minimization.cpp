#include <iostream>
#include <cmath>
#include <fstream>
#include "simulated_annealing.h"

const double initial_x = 15.5; // 定义起始搜索点
const unsigned iteration_times = 100000; // 定义迭代次数
const unsigned initial_step = 3741121; // 定义退火函数的

const double energy(const double x) {
	// 定义目标函数。
	return -(12 * std::pow(x, 5) - 975 * std::pow(x, 4) + 28000 * std::pow(x, 3) - 345000 * std::pow(x, 2) + 1800000 * x);
}

const double temperature(const int c) {
	// 退火函数
	return  initial_step * std::pow(0.5, c - 1);
}

const double next(double x, std::default_random_engine& g) {
	std::uniform_real_distribution<double> dist(-1.0, 1.0);
	return x + dist(g);
}

int minimization() {
	std::default_random_engine generator;

	double result = simulated_annealing<double, int, decltype(&energy), decltype(&temperature), decltype(&next), std::default_random_engine>
		(initial_x, iteration_times, energy, temperature, next, generator);

	std::ofstream output("sa_output.txt");
	if (output) {
		std::cout << "结果已写入 sa_output.txt 文件" << std::endl;
		output << result << " <-- 最优状态" << '\n' << energy(result) << " <-- 最优解" << '\n' << '\n'
			<< initial_x << " <-- 起始状态" << '\n' << energy(initial_x) << " <-- 起始解" << '\n'
			<< iteration_times << " <-- 迭代次数" << std::endl;
	}
	else {
		std::cout << "无法写入 sa_output.txt 文件" << '\n';
		std::cout << result << " <-- 最优状态" << '\n' << energy(result) << " <-- 最优解" << '\n' << '\n'
			<< initial_x << " <-- 起始状态" << '\n' << energy(initial_x) << " <-- 起始解" << '\n'
			<< iteration_times << " <-- 迭代次数" << std::endl;

		std::cin >> result; // 窗口停留
	}
	output.close();
	return 0;
}