#include <iostream>
#include <algorithm>
#include <random>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <numeric>
#include <iterator>
#include "../tools/data_processing.h"
#include "random_hash_cluster.h"

void k_means_example();


int main()
{
	matools::VectorDB db("data.txt");
	std::cout << db.data() << std::endl;
	std::ostream_iterator<bool> os_it(std::cout, " ");
	for (auto i : db.m_hash_val) {
		std::copy(i.cbegin(), i.cend(), os_it);
		std::cout << '\n';
	}
	std::cout << '\n';
	std::vector<bool> value = db.get_hash_val("hwllo");
	std::copy(value.begin(), value.end(), os_it);
	std::vector<Eigen::MatrixXd::Index> deleted_rows{1, 2};
	db.erase_rows(deleted_rows);
	db.save_to(std::cout);
	std::cin.get();
}









void k_means_example()
{
	std::string filename;
	std::ifstream infile;

	std::cout << "Enter your data file name:" << std::endl;
	while (std::cin >> filename) {
		infile.open(filename);
		if (infile) {
			break;
		}

		std::cout << "Unable to open " << filename << "\nTry again:" << std::endl;
	}

	std::vector<double> data;
	while (infile) {
		double temp;
		infile >> temp;
		data.push_back(temp);
	}

	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0, data.size());
	int random_a = distribution(generator), random_b = distribution(generator);
	while (random_a == random_b) {
		random_b = distribution(generator);
	}

	double a = data[random_a], b = data[random_b];
	std::vector<double> cluster_a, cluster_b;
	for (unsigned n = 20; n != 0; --n) {
		cluster_a = {};
		cluster_b = {};

		std::partition_copy(data.begin(), data.end(), std::back_inserter(cluster_a), std::back_inserter(cluster_b), [&a, &b](double i) -> bool {
			return std::abs(i - a) > std::abs(i - b);
			});

		a = std::accumulate(cluster_a.begin(), cluster_a.end(), 0) / cluster_a.size();
		b = std::accumulate(cluster_b.begin(), cluster_b.end(), 0) / cluster_b.size();
	}

	std::ofstream out("output.txt");
	for (double i : cluster_a) {
		out << i << ' ';
	}
	out << "<-- Cluster 1\n";
	for (double i : cluster_b) {
		out << i << ' ';
	}
	out << "<-- Cluster 2" << std::endl;
}