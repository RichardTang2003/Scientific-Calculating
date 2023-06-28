#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "../../deps/Eigen/Dense"

std::vector<std::vector<unsigned>>& read(std::string filename, std::vector<std::vector<unsigned>>& data);
Eigen::MatrixXd loadMatrix(const std::string& filename);
#endif
