#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "../../deps/Eigen/Dense"

std::vector<std::vector<unsigned>>& read(std::string filename, std::vector<std::vector<unsigned>>& data) {
	std::ifstream infile(filename);

	if (!infile) {
		std::cerr << "Unable to open file:" << filename << std::endl;
	}

	std::string line;
	while (std::getline(infile, line)) {
		std::vector<unsigned> row;
		std::istringstream iss(line);
		unsigned value;

		while (iss >> value) {
			row.push_back(value);
		}

		data.push_back(row);
	}

#ifdef _DEBUG
	for (const auto& row : data) {
		for (const auto& val : row) {
			std::cout << val << ' ';
		}
		std::cout << std::endl;
	}
#endif

	return data;
}

Eigen::MatrixXd loadMatrix(const std::string& filename) {
	std::vector<double> data;
	int rows = 0;
	int cols = 0;

	std::ifstream infile(filename);
	if (!infile) {
		std::cerr << "Unable to open file:" << filename << std::endl;
	}

	while (infile) {
		std::string line;
		if (!getline(infile, line)) break;

		std::istringstream ss(line);
		cols = 0;
		while (ss) {
			std::string s;
			if (!getline(ss, s, ',')) break;
			data.push_back(stod(s));
			cols++;
		}
		rows++;
	}

	return Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(data.data(), rows, cols);
}