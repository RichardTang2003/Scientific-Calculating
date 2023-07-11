#include <iostream>
#include <fstream>
#include <ostream>
#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include <sstream>
#include "../tools/data_processing.h"
#include "../../deps/Eigen/Dense"
#include "random_hash_cluster.h"

namespace matools
{
	VectorDB::VectorDB(const std::string& filename)
	{
		m_data = load_name_matrix(filename, m_label);
		m_dimension = m_data.cols();
		generate_hash_rules(m_dimension);
		m_hash_val.resize(m_label.size());
#pragma omp parallel for
		for (Eigen::MatrixXd::Index i = 0; i <= m_label.size() - 1; ++i) {
			m_row_label_table.emplace(m_label[i], i);
			m_hash_val[i] = hash(m_data.row(i));
		}

	}

	VectorDB::VectorDB(Eigen::MatrixXd&& m)
	{
		m_data = std::move(m);
		m_dimension = m_data.cols();
		generate_hash_rules(m_dimension);
		m_hash_val.resize(m_data.rows());
#pragma omp parallel for
		for (Eigen::MatrixXd::Index i = 0; i <= m_data.rows() - 1; ++i) {
			m_row_label_table.emplace(std::to_string(i), i);
			m_hash_val[i] = hash(m_data.row(i));
		}

	}

	VectorDB& VectorDB::save_to(std::ostream& os)
	{
		std::ostream_iterator<std::string> os_iter(os, " ");
		for (Eigen::MatrixXd::Index i = 0; i != m_data.rows(); ++i)
		{
			os << m_label[i] << ' ';
			os << m_data.row(i) << '\n';
		}

		return *this;
	}

	VectorDB& VectorDB::save_to(const std::string& filename)
	{
		std::ofstream out(filename);
		if (!out) {
			std::cout << "Unable to open \"" << filename << "\" ...";
			return *this;
		}
		return save_to(out);

	}

	VectorDB& VectorDB::load_from(const std::string& filename)
	{
		clear();
		m_data = load_name_matrix(filename, m_label);
		m_dimension = m_data.cols();
		generate_hash_rules(m_dimension);
		m_hash_val.resize(m_label.size());
#pragma omp parallel for
		for (Eigen::MatrixXd::Index i = 0; i <= m_label.size(); ++i) {
			m_row_label_table.emplace(m_label[i], i);
			m_hash_val[i] = hash(m_data.row(i));
		}
		return *this;
	}

	VectorDB& VectorDB::set_label(const std::string& key, const std::string& new_name)
	{
		if (m_row_label_table.find(key) == m_row_label_table.end())
		{
			std::cout << "Warning: No matching entry \"" << key << "\" found in database!\n";
			return *this;
		}

		Eigen::MatrixXd::Index i = m_row_label_table.at(key);
		m_row_label_table.erase(key);
		m_row_label_table.emplace(new_name, i);
	}

	Eigen::MatrixXd VectorDB::load_name_matrix(const std::string& filename, std::vector<std::string>& row_label, const char delimiter)
	{
		row_label = {};

		std::ifstream in(filename);
		if (!in)
		{
			std::cerr << "Unable to read " << filename << "...\n";
			return Eigen::MatrixXd();
		}

		std::string line;
		Eigen::MatrixXd::Index cols = 0;
		std::vector<double> data;
		bool firstLine = true;
		while (std::getline(in, line))
		{
			std::string label;
			std::istringstream ss(line);
			std::getline(ss, label, delimiter);
			row_label.push_back(label);

			std::string value;
			Eigen::MatrixXd::Index temp_cols = 0;
			while (std::getline(ss, value, delimiter))
			{
				std::istringstream ss(value);
				double number;
				ss >> number;
				data.push_back(number);
				++temp_cols;
			}

			if (firstLine)
			{
				cols = temp_cols;
				firstLine = false;
			}
			else if (cols != temp_cols)
			{
				std::cerr << "Inconsistent number of columns in " << filename << "...\n";
				return Eigen::MatrixXd();
			}
		}

		return Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(data.data(), row_label.size(), cols);
	}
}