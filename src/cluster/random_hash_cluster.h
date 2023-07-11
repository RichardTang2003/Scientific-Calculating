#ifndef RANDOM_HASH_CLUSTER_H
#define RANDOM_HASH_CLUSTER_H
#include <iostream>
#include <fstream>
#include <ostream>
#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include <sstream>
#include <memory>
#include "../tools/data_processing.h"
#include "../../deps/Eigen/Dense"

namespace matools
{

	class VectorDB
	{
	public:
		VectorDB() = default;
		VectorDB(const std::string& filename);
		VectorDB(const Eigen::MatrixXd&& m);

		VectorDB& operator>>(std::ostream& os) { return save_to(os); };


		Eigen::MatrixXd& data() { return m_data; }
		Eigen::VectorXd get_vector(std::string& key)
		{
			if (m_row_label_table.find(key) != m_row_label_table.end())
				return m_data.row(m_row_label_table[key]);
			else
			{
				std::cout << "Warning: No matching entry \"" << key << "\" found in database!\n";
				static Eigen::Vector<double, 0> empty_vector;
				return empty_vector;
			}
		}
		VectorDB& set_label(const std::string& old_name, const std::string& new_name);
		VectorDB& save_to(std::ostream& os);
		VectorDB& load_from(std::istream& is);
		VectorDB& push_back(const std::string& key, const Eigen::Matrix<double, 1, Eigen::Dynamic>& data);
		VectorDB& erase(const std::string& key);
		VectorDB& z_score_normalize()
		{
			z_score(m_data);
			return *this;
		}
		std::vector<bool>& get_hash_val(const std::string& key)
		{
			if (m_row_label_table.find(key) != m_row_label_table.end())
				return m_hash_val[m_row_label_table[key]];
			else
			{
				std::cout << "Warning: No matching entry \"" << key << "\" found in database!\n";
				static std::vector<bool> empty_vector;
				return empty_vector;
			}
		}

		//private:
		Eigen::MatrixXd m_data;
		Eigen::MatrixXd m_hash_rules;
		std::unordered_map<std::string, typename Eigen::MatrixXd::Index> m_row_label_table;
		std::vector<std::vector<bool>> m_hash_val;
		std::vector<std::string> m_label;
		Eigen::MatrixXd::Index m_dimension;

		std::vector<bool> hash(const Eigen::VectorXd& it)
		{
			std::vector<bool> hash_value;
			for (const auto& vector : m_hash_rules.rowwise())
			{
				bool which_side = (it.dot(vector) >= 0);
				hash_value.emplace_back(which_side);
			}

			return hash_value;
		}
		void generate_hash_rules(const Eigen::MatrixXd::Index& bits)
		{
			std::srand((unsigned int)std::time(0));
			m_hash_rules = Eigen::MatrixXd::Random(bits, m_dimension);
		}

		Eigen::MatrixXd load_name_matrix(const std::string& filename, std::vector<std::string>& row_label, const char delimiter = ',');


	};






}

#endif