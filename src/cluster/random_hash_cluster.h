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
		Eigen::MatrixXd m_data;
		Eigen::MatrixXd m_hash_rules;
		std::unordered_map<std::string, typename Eigen::MatrixXd::Index> m_row_label_table;
		std::vector<std::vector<bool>> m_hash_val;
		std::vector<std::string> m_label;
		Eigen::MatrixXd::Index m_dimension;

	public:
		VectorDB() = default;
		VectorDB(const std::string& filename);
		VectorDB(Eigen::MatrixXd&& m);
		VectorDB(VectorDB& that) = delete;

		VectorDB& operator=(VectorDB& that) = delete;
		VectorDB& operator>>(std::ostream& os) { return save_to(os); };
		bool operator==(const VectorDB& that) const { return m_hash_val == that.m_hash_val; }

		Eigen::MatrixXd& data() { return m_data; }
		Eigen::VectorXd get_vector(const std::string& key);
		double get_value(const std::string& key, const Eigen::MatrixXd::Index index);
		std::vector<bool>& get_hash_val(const std::string& key);
		VectorDB& save_hash_table_to(std::ostream& os);
		VectorDB& save_hash_table_to(const std::string& filename);
		Eigen::MatrixXd::Index get_dimension() { return m_dimension; }
		bool has(const std::string& key) { return m_row_label_table.find(key) != m_row_label_table.end(); }
		VectorDB& set_label(const std::string& old_name, const std::string& new_name);
		VectorDB& save_to(std::ostream& os);
		VectorDB& save_to(const std::string& filename);
		VectorDB& load_from(const std::string& filename);
		VectorDB& push_back(const std::string& key, const Eigen::RowVectorXd& data);
		VectorDB& erase(const std::string& key);
		VectorDB& erase(const Eigen::MatrixXd::Index& index);
		VectorDB& erase_rows(std::vector<typename Eigen::MatrixXd::Index>& rowIndices);
		VectorDB& erase_rows(const std::vector<std::string>& keys);
		VectorDB& rehash(const uint32_t times);

		VectorDB& z_score_normalize() { z_score(m_data); return *this; }
		VectorDB& clear()
		{
			Eigen::MatrixXd().swap(m_data);
			Eigen::MatrixXd().swap(m_hash_rules);
			std::unordered_map<std::string, typename Eigen::MatrixXd::Index>().swap(m_row_label_table);
			std::vector<std::vector<bool>>().swap(m_hash_val);
			std::vector<std::string>().swap(m_label);
			m_dimension = 0;
			return *this;
		}

	private:
		std::vector<bool> hash(const Eigen::VectorXd& it);
		void generate_hash_rules(const Eigen::MatrixXd::Index& bits);

		Eigen::MatrixXd load_name_matrix(const std::string& filename, std::vector<std::string>& row_label, const char delimiter = ',');


	};

	namespace VectorDB_client
	{
		class MenuBase
		{
		protected:
			VectorDB* m_db_ptr;
			MenuBase* m_upper_ptr;
			std::string m_header;
			std::string m_help;
			std::string m_message;

			MenuBase(VectorDB& db, MenuBase* upper_ptr, std::string message, std::string help, std::string header) :
				m_db_ptr(&db), m_upper_ptr(upper_ptr), m_help(help), m_message(message), m_header(header) {}

			MenuBase(VectorDB& db, MenuBase* upper_ptr, std::string message, std::string help) :
				m_db_ptr(&db), m_upper_ptr(upper_ptr), m_help(help), m_message(message), m_header() {}

		public:
			virtual MenuBase* get(bool& go_back) = 0;
			MenuBase* exit()
			{
				return m_upper_ptr;
			}
			MenuBase& show();
			void line() { std::cout << "------------------------\n"; }
		};

		class MainMenu : public MenuBase
		{
		public:
			MainMenu(VectorDB& db) :
				MenuBase(db, nullptr,
					"Choose a menu:\n1. File IO\n2. Operate your data\n3. Do some calculation.\n9. Back",
					"You can enter number to open the operation menu, File IO provides some function to interact with files, OperateMenu helps you directly manipulate your data and Calculation Menu perform some scientific calculation.",
					"Vector DataBase Client | illlights presents.\nThis is a Vector DataBase demo, try it out!\nYou can always enter \"help\" for help!\n") { }
			MenuBase* get(bool& go_back) override;
		};

		class FileMenu : public MenuBase
		{

		public:
			FileMenu(VectorDB& db, MenuBase* upper_ptr) :
				MenuBase(db, upper_ptr,
					"Now choose what you want to do:\n1. Load data from File\n2. Save data to File\n9. Back",
					"You can enter number to operate.") { }
			MenuBase* get(bool& go_back) override;
		};

		class OperateMenu : public MenuBase
		{

		public:
			OperateMenu(VectorDB& db, MenuBase* upper_ptr) :
				MenuBase(db, upper_ptr,
					"Now choose what you want to do:\n1. Add data\n2. Erase data\n3. Show all data\n4. Find data\n5. Get hash table\n6. Get hash value\n9. Back",
					"You can enter number to operate.") { }
			MenuBase* get(bool& go_back) override;
		};

		class CalculateMenu : public MenuBase
		{

		public:
			CalculateMenu(VectorDB& db, MenuBase* upper_ptr) :
				MenuBase(db, upper_ptr,
					"Now choose what you want to do:\n1. Perform z-score on data\n9. Back",
					"You can enter number to operate.") { }
			MenuBase* get(bool& go_back) override;
		};
	};

}

#endif