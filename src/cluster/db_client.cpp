#include <string>
#include <iostream>
#include <sstream>
#include "random_hash_cluster.h"


namespace matools::VectorDB_client
{
	MenuBase& MenuBase::show()
	{
		if (!m_header.empty())
		{
			std::cout << m_header << '\n';
			line();
		}
		std::cout << m_message << '\n';
		return *this;
	}

	MenuBase* MainMenu::get(bool& go_back)
	{

		if (!std::isdigit(std::cin.peek()))
		{
			std::string input;
			std::cin >> input;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			if (input == std::string("help"))
				std::cout << '\n' << m_help << '\n';
			else {
				std::cout << "\nError Input, Try again:\n";
				return get(go_back);
			}
		}

		int input;
		std::cin >> input;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		switch (input)
		{
		case 1:

			return new FileMenu(*m_db_ptr, this);
		case 2:

			return new OperateMenu(*m_db_ptr, this);
		case 3:

			return new CalculateMenu(*m_db_ptr, this);
		case 9:
			go_back = 1;

			return m_upper_ptr;
		default:
			std::cout << "\nError Input, Try again:\n";

			return get(go_back);
		}

	}

	MenuBase* FileMenu::get(bool& go_back)
	{
		if (!std::isdigit(std::cin.peek()))
		{
			std::string input;
			std::cin >> input;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			if (input == std::string("help"))
				std::cout << '\n' << m_help << '\n';
			else {
				std::cout << "\nError Input, Try again:\n";
				return FileMenu::get(go_back);
			}
		}

		int input;
		std::cin >> input;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::string filename;
		switch (input)
		{
		case 1:
			std::cout << "Enter your filename:\n";
			while (std::cin >> filename)
			{
				std::ifstream in(filename);
				if (!in)
				{
					std::cout << "Error Open File " << filename << ", try again:\n";
					continue;
				}
				in.close();
				m_db_ptr->load_from(filename);
				std::cout << "Success!\n";
				break;
			}
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			return this;
		case 2:
			std::cout << "Enter your filename:\n";
			while (std::cin >> filename)
			{
				std::ofstream out(filename);
				if (!out)
				{
					std::cout << "Error Open File " << filename << ", try again:\n";
					continue;
				}
				out.close();
				m_db_ptr->save_to(filename);
				std::cout << "Success!\n";
				break;
			}
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			return this;
		case 9:
			go_back = 1;

			return m_upper_ptr;
		default:
			std::cout << "\nError Input, Try again:\n";

			return get(go_back);
		}

	}

	MenuBase* OperateMenu::get(bool& go_back)
	{
		if (!std::isdigit(std::cin.peek()))
		{
			std::string input;
			std::cin >> input;
			if (input == std::string("help"))
				std::cout << '\n' << m_help << '\n';
			else {
				std::cout << "\nError Input, Try again:\n";
				return get(go_back);
			}
		}

		int input;
		std::string input_str;
		std::cin >> input;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		switch (input)
		{
		case 1:
			std::cout << "Now enter your formatted data, enter \"help\" for formatted information.\n";
			while (std::getline(std::cin, input_str))
			{
				std::istringstream ss(input_str);
				if (!std::isalpha(input_str[0]))
				{
					std::cout << "\nError Input, Try again:\n";
					continue;
				}

				if (input_str == "help")
				{
					std::cout << "You need to enter only one line of data, Format: [key] [float] [float] [float]...\nAttention! only one line is accepted and dimension must correspond to original data.\nAttention! first character should be letter!\n";
					continue;
				}

				std::string key;
				ss >> key;
				std::vector<double> data;
				double value;
				while (ss >> value)
				{
					data.push_back(value);
				}

				if (data.size() != m_db_ptr->get_dimension())
				{
					std::cout << "\nError Dimension, dimension should be " << m_db_ptr->get_dimension() << ", Try again : \n";
					continue;
				}

				Eigen::RowVectorXd vec(data.size());
				vec = Eigen::VectorXd::Map(&data[0], data.size());
				m_db_ptr->push_back(key, vec);
				std::cout << "Success!" << std::endl;
				break;
			}
			return this;
		case 2:
			std::cout << "Which data do you want to delete? Enter the name of that data.\n";
			while (std::getline(std::cin, input_str))
			{
				if (!std::isalpha(input_str[0]))
				{
					std::cout << "\nError Input, Try again:\n";
					continue;
				}

				if (input_str == "help")
				{
					std::cout << "Just enter a string started with letter!\n";
					continue;
				}

				if (!m_db_ptr->has(input_str))
				{
					std::cout << "data named \"" << input_str << "\"not find, try again:\n";
					continue;
				}

				m_db_ptr->erase(input_str);
				std::cout << "Success!" << std::endl;
				break;
			}
			return this;
		case 3:
			m_db_ptr->save_to(std::cout);
			return this;
		case 4:
			std::cout << "Enter the name of that data.\n";
			while (std::getline(std::cin, input_str))
			{
				if (!std::isalpha(input_str[0]))
				{
					std::cout << "\nError Input, Try again:\n";
					continue;
				}

				if (input_str == "help")
				{
					std::cout << "Just enter a string started with letter!\n";
					continue;
				}

				if (!m_db_ptr->has(input_str))
				{
					std::cout << "data named \"" << input_str << "\"not find, try again:\n";
					continue;
				}

				std::cout << m_db_ptr->get_vector(input_str);
				break;
			}
			return this;
		case 5:
			m_db_ptr->save_hash_table_to(std::cout);
			std::cout << "Save this to file? y/n\n";
			char decision;
			std::cin >> decision;
			if (decision == 'y' || decision == 'Y') {
				std::string filename;
				std::cout << "Enter your filename:\n";
				while (std::cin >> filename)
				{
					std::ofstream out(filename);
					if (!out)
					{
						std::cout << "Error Open File " << filename << ", try again:\n";
						continue;
					}
					m_db_ptr->save_hash_table_to(out);
					std::cout << "Success!\n";
					break;
				}
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				return this;
			}
			std::cout << "ok!\n";
			return this;
		case 6:
			std::cout << "Enter the name of that data.\n";
			while (std::getline(std::cin, input_str))
			{
				if (!std::isalpha(input_str[0]))
				{
					std::cout << "\nError Input, Try again:\n";
					continue;
				}

				if (input_str == "help")
				{
					std::cout << "Just enter a string started with letter!\n";
					continue;
				}

				if (!m_db_ptr->has(input_str))
				{
					std::cout << "data named \"" << input_str << "\"not find, try again:\n";
					continue;
				}

				auto& hash_value = m_db_ptr->get_hash_val(input_str);
				std::ostream_iterator<bool> os(std::cout, " ");
				std::copy(hash_value.cbegin(), hash_value.cend(), os);
				break;
			}
			return this;
		case 9:
			go_back = 1;

			return m_upper_ptr;
		default:
			std::cout << "\nError Input, Try again:\n";

			return get(go_back);
		}

	}

	MenuBase* CalculateMenu::get(bool& go_back)
	{
		if (!std::isdigit(std::cin.peek()))
		{
			std::string input;
			std::cin >> input;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			if (input == std::string("help"))
				std::cout << '\n' << m_help << '\n';
			else {
				std::cout << "\nError Input, Try again:\n";
				return get(go_back);
			}
		}

		int input;
		std::cin >> input;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::string filename;
		switch (input)
		{
		case 1:
			m_db_ptr->z_score_normalize();
			std::cout << "Success!" << '\n';
			return this;

		case 9:
			go_back = 1;
			return m_upper_ptr;
		default:
			std::cout << "\nError Input, Try again:\n";

			return get(go_back);
		}

	}
}