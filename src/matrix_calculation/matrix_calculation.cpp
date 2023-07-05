#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <random>
#include "../../deps/Eigen/Dense"
#include "../tools/data_processing.h"

using Eigen::Matrix;

int xA_calculation()
{
	std::cout << "This is a program calculating x * A^n\n"
		<< "Attention: Plain text delimited by ' ' or ',' are ok.\n"
		<< "-----------------------------------------------\n"
		<< "Enter your filename for RawVector x:" << std::endl;
	std::string vector_file_name;
	std::ifstream vector;
	while (std::cin >> vector_file_name) {
		vector.open(vector_file_name);
		if (vector)
			break;

		std::cout << "Unable to open '" << vector_file_name << "' ...\nTry again:" << std::endl;
	}
	Eigen::RowVectorXd x = load_matrix<double>(vector_file_name);

	std::cout << "Okay, now enter your filename for Matrix A:" << std::endl;
	std::string matrix_file_name;
	std::ifstream matrix;
	while (std::cin >> matrix_file_name) {
		matrix.open(matrix_file_name);
		if (matrix)
			break;

		std::cout << "Unable to open " << matrix_file_name << "\nTry again:" << std::endl;
	}
	Eigen::MatrixXd m = load_matrix<double>(matrix_file_name);

	std::cout << "Last step, enter the exponent n, only integers are allowed:" << std::endl;
	int n;
	std::cin >> n;

	for (int i = 0; i != n; ++i) {
		Eigen::RowVectorXd temp = x * m;
		x = temp;
	}

	std::ofstream out("result.txt");
	if (!out) {
		std::cerr << "Unable to write 'result.txt', try to use administration mode to run this program." << std::endl;
		std::cin >> n;
		return -1;
	}
	out << x << std::endl;

}

int main()
{
	std::cout << "This is a program calculating cov matrix\n"
		<< "-----------------------------------------------\n"
		<< "Okay, now enter your filename for Matrix A:" << std::endl;
	std::string matrix_file_name;
	std::ifstream matrix;
	while (std::cin >> matrix_file_name) {
		matrix.open(matrix_file_name);
		if (matrix)
			break;

		std::cout << "Unable to open " << matrix_file_name << "\nTry again:" << std::endl;
	}
	Eigen::MatrixXd m = load_matrix<double>(matrix_file_name);

	std::ofstream out("output.txt");

	auto start = std::chrono::high_resolution_clock::now();
	out << z_score(m) << std::endl;
	auto end = std::chrono::high_resolution_clock::now();
	// 计算时间差
	std::chrono::duration<double> duration = end - start;


	return 0;
}

