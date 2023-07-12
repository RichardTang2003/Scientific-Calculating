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
		for (Eigen::MatrixXd::Index i = 0; i < m_label.size(); ++i) {
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
		for (Eigen::MatrixXd::Index i = 0; i < m_data.rows(); ++i) {
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
		for (Eigen::MatrixXd::Index i = 0; i < m_label.size(); ++i) {
			m_row_label_table.emplace(m_label[i], i);
			m_hash_val[i] = hash(m_data.row(i));
		}
		return *this;
	}

	inline Eigen::VectorXd VectorDB::get_vector(const std::string& key)
	{
		if (m_row_label_table.find(key) != m_row_label_table.end())
		{
			return m_data.row(m_row_label_table[key]);
		}
		else
		{
			std::cout << "Warning: No matching entry \"" << key << "\" found in database!\n";
			static Eigen::Vector<double, 0> empty_vector;
			return empty_vector;
		}
	}

	inline double VectorDB::get_value(const std::string& key, const Eigen::MatrixXd::Index index)
	{
		if (m_row_label_table.find(key) != m_row_label_table.end())
		{
			if (index <= m_dimension)
			{
				return m_data(m_row_label_table[key], index);
			}
			else
			{
				std::cout << "Warning: Given index out of range " << m_dimension << "\n";
				return 0;
			}
		}
		else
		{
			std::cout << "Warning: No matching entry \"" << key << "\" found in database!\n";
			return 0;
		}
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

	VectorDB& VectorDB::rehash(const uint32_t times)
	{
		generate_hash_rules(times);
		std::vector<std::vector<bool>>().swap(m_hash_val);
		m_hash_val.resize(m_data.rows());
#pragma omp parallel for
		for (Eigen::MatrixXd::Index i = 0; i <= m_data.rows() - 1; ++i) {
			m_hash_val[i] = hash(m_data.row(i));
		}
	}

	VectorDB& VectorDB::erase(const std::string& key)
	{
		if (m_row_label_table.find(key) == m_row_label_table.end())
		{
			std::cout << "Warning: No matching entry \"" << key << "\" found in database!\n";
			return *this;
		}
		Eigen::MatrixXd::Index rowIndex = m_row_label_table[key];
		return erase(rowIndex);
	}

	VectorDB& VectorDB::erase(const Eigen::MatrixXd::Index& rowIndex)
	{

		if (rowIndex > m_data.rows())
		{
			std::cout << "Warning: No matching entry \"" << rowIndex << "\" found in database!\n";
			return *this;
		}

		Eigen::MatrixXd reducedMat(m_data.rows() - 1, m_data.cols()); // 创建一个新的矩阵，行数减一

		// 将删除行之前的所有行复制到新矩阵中
		if (rowIndex > 0)
		{
			reducedMat.block(0, 0, rowIndex, m_data.cols()) = m_data.block(0, 0, rowIndex, m_data.cols());
		}

		// 如果删除的不是最后一行，将删除行之后的所有行上移
		if (rowIndex < m_data.rows() - 1)
		{
			reducedMat.block(rowIndex, 0, m_data.rows() - rowIndex - 1, m_data.cols()) = m_data.block(rowIndex + 1, 0, m_data.rows() - rowIndex - 1, m_data.cols());
		}

		m_data.swap(reducedMat);
		m_label.erase(m_label.begin() + rowIndex);
		m_hash_val.erase(m_hash_val.begin() + rowIndex);

		//重新生成对应 table
		std::unordered_map<std::string, typename Eigen::MatrixXd::Index>().swap(m_row_label_table);

		for (Eigen::MatrixXd::Index i = 0; i < m_label.size(); ++i) {
			m_row_label_table.emplace(m_label[i], i);
		}

		return *this;
	}


	VectorDB& VectorDB::erase_rows(const std::vector<std::string>& keys)
	{
		std::vector<typename Eigen::MatrixXd::Index> rowIndices;

		for (const auto& key : keys)
		{
			auto it = m_row_label_table.find(key);
			if (it == m_row_label_table.end())
			{
				std::cout << "Warning: No matching entry \"" << key << "\" found in database!\n";
			}
			else
			{
				rowIndices.push_back(it->second);
			}
		}

		return erase_rows(rowIndices);
	}



	VectorDB& VectorDB::erase_rows(std::vector<typename Eigen::MatrixXd::Index>& row_indices)
	{
		// 对要删除的行进行排序并去重
		std::vector<typename Eigen::MatrixXd::Index> rowIndices = row_indices;
		std::sort(rowIndices.begin(), rowIndices.end());
		rowIndices.erase(std::unique(rowIndices.begin(), rowIndices.end()), rowIndices.end());

		// 检查是否所有的行索引都合法
		for (auto rowIndex : rowIndices)
		{
			if (rowIndex >= m_data.rows() || rowIndex < 0)
			{
				std::cout << "Warning: Invalid row index \"" << rowIndex << "\" in database!\n";
				return *this;
			}
		}

		// 创建新的矩阵
		Eigen::MatrixXd reducedMat(m_data.rows() - rowIndices.size(), m_data.cols());

		// 记录当前要复制到新矩阵的位置
		typename Eigen::MatrixXd::Index currentRow = 0;

		for (typename Eigen::MatrixXd::Index i = 0; i < m_data.rows(); ++i)
		{
			// 如果这一行在要删除的行中，则跳过
			if (std::binary_search(rowIndices.begin(), rowIndices.end(), i))
			{
				continue;
			}

			// 否则，复制这一行到新矩阵
			reducedMat.row(currentRow++) = m_data.row(i);
		}

		m_data.swap(reducedMat);

		for (auto it = rowIndices.rbegin(); it != rowIndices.rend(); ++it)
		{
			m_label.erase(m_label.begin() + *it);
			m_hash_val.erase(m_hash_val.begin() + *it);
		}

		//重新生成对应table
		std::unordered_map<std::string, typename Eigen::MatrixXd::Index>().swap(m_row_label_table);

		for (Eigen::MatrixXd::Index i = 0; i < m_label.size(); ++i) {
			m_row_label_table.emplace(m_label[i], i);
		}

		return *this;
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