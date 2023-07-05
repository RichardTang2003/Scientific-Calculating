#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "../../deps/Eigen/Dense"

template <typename T>
std::vector<std::vector<T>> load_txt_to_vector(const std::string& filename)
{
	std::ifstream infile(filename);

	if (!infile) {
		std::cerr << "Unable to open file: " << filename << std::endl;
	}

	std::string line;
	std::getline(infile, line);
	std::istringstream iss(line);

	// 先读取第一行，存储到一个临时的 vector<T> 中,确定列数
	std::vector<T> temp;
	T value;
	while (iss >> value) {
		temp.push_back(value);
	}
	int num_cols = temp.size();

	// 创建一个 num_cols 大小的二维 vector，每一列都对应一个 vector<T>
	std::vector<std::vector<T>> data(num_cols);

	// 将第一行的数据加入到相应的 vector 中
	for (int i = 0; i < num_cols; ++i) {
		data[i].push_back(temp[i]);
	}

	// 读取剩余的行
	while (std::getline(infile, line)) {
		std::istringstream iss(line);

		for (int i = 0; i < num_cols; i++) {
			if (iss >> value) {
				data[i].push_back(value);
			}
		}
	}

#ifdef _DEBUG
	for (const auto& row : data) {
		for (const auto& val : row) {
			std::cout << val << ' ';
		}
		std::cout << '\n';
	}
#endif

	return data;
}

template <typename T>
std::vector<std::vector<T>> load_vector(const std::string& filename, const char delimiter = ',')
{
	std::ifstream infile(filename);

	if (!infile) {
		std::cerr << "Unable to open file: " << filename << std::endl;
	}

	std::string line;
	std::getline(infile, line);
	if (std::count(line.begin(), line.end(), delimiter) == 0) {
		return load_txt_to_vector<T>(filename);
	}


	std::vector<std::vector<T>> data;

	while (std::getline(infile, line)) {
		std::istringstream iss(line);
		std::vector<T> row;
		T value;

		while (iss >> value) {
			row.push_back(value);

			if (iss.peek() == delimiter) {
				iss.ignore();
			}
		}

		data.push_back(row);
	}

#ifdef _DEBUG
	for (const auto& row : data) {
		for (const auto& val : row) {
			std::cout << val << ' ';
		}
		std::cout << '\n';
	}
#endif

	return data;
}

template <typename T>
Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> load_txt_to_matrix(const std::string& filename)
{
	std::vector<T> data;
	int rows = 0;
	int cols = 0;

	std::ifstream infile(filename);
	if (!infile) {
		std::cerr << "Unable to open file:" << filename << std::endl;
		return Eigen::MatrixXd();
	}

	std::string line;
	while (std::getline(infile, line)) {
		std::istringstream ss(line);
		T value;
		cols = 0;
		while (ss >> value) {
			data.push_back(value);
			cols++;
		}
		rows++;
	}

	return Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(data.data(), rows, cols);
}

template <typename T>
Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> load_matrix(const std::string& filename, const char delimiter = ',')
{
	std::ifstream infile(filename);
	if (!infile) {
		std::cerr << "Unable to open file: " << filename << std::endl;
		return Eigen::MatrixXd();
	}

	std::string line;
	std::getline(infile, line);
	if (std::count(line.begin(), line.end(), delimiter) == 0) {
		infile.close();
		return load_txt_to_matrix<T>(filename);
	}

	std::vector<T> data;
	int rows = 0;
	int cols = 0;
	do {
		std::istringstream ss(line);
		std::string cell;
		cols = 0;
		while (std::getline(ss, cell, delimiter)) {
			std::istringstream cell_ss(cell);
			T value;
			if (cell_ss >> value) {
				data.push_back(value);
				cols++;
			}
			else {
				std::cerr << "Invalid data found in file: " << filename << std::endl;
				return Eigen::MatrixXd();
			}
		}
		rows++;
	} while (std::getline(infile, line));

	return Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(data.data(), rows, cols);
}

enum class OperationAxis {
	Row = 0,
	Col = 1,
};

template<typename Derived>
inline typename Derived::Scalar std_deviation(const Eigen::MatrixBase<Derived>& m, OperationAxis axis = OperationAxis::Col, typename Derived::Index index = 0) {
	typename Derived::Scalar mean, stdDev;

	if (axis == OperationAxis::Col) {
		mean = m.col(index).mean();
		stdDev = std::sqrt((m.col(index).array() - mean).square().sum() / (m.rows() - 1));
	}
	else {
		mean = m.row(index).mean();
		stdDev = std::sqrt((m.row(index).array() - mean).square().sum() / (m.cols() - 1));
	}

	return stdDev;
}

template<typename Derived>
Eigen::MatrixBase<Derived>& z_score(Eigen::MatrixBase<Derived>& m, OperationAxis axis = OperationAxis::Col) {
	typename Derived::Index size = axis == OperationAxis::Col ? m.cols() : m.rows();

	for (typename Derived::Index i = 0; i < size; ++i) {
		typename Derived::Scalar stdDev = std_deviation(m, axis, i);

		if (axis == OperationAxis::Col) {
			typename Derived::Scalar mean = m.col(i).mean();
			m.col(i) = (m.col(i).array() - mean) / stdDev;
		}
		else {
			typename Derived::Scalar mean = m.row(i).mean();
			m.row(i) = (m.row(i).array() - mean) / stdDev;
		}
	}

	return m;
}




#endif // DATA_PROCESSING_H
