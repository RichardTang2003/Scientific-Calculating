#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>

std::vector<double> entropy_weight(const std::vector<std::vector<double>>& matrix) {
    size_t n_alternatives = matrix.size();
    size_t n_criteria = matrix[0].size();

    // Normalize data
    std::vector<std::vector<double>> normalized_matrix(n_alternatives, std::vector<double>(n_criteria, 0.0));
    for (size_t j = 0; j < n_criteria; ++j) {
        double sum = 0.0;
        for (const auto& row : matrix) {
            sum += row[j];
        }
        for (size_t i = 0; i < n_alternatives; ++i) {
            normalized_matrix[i][j] = matrix[i][j] / sum;
        }
    }

    // Calculate entropy for each criterion
    std::vector<double> entropy(n_criteria, 0.0);
    for (size_t j = 0; j < n_criteria; ++j) {
        for (const auto& row : normalized_matrix) {
            if (row[j] > 0.0) {
                entropy[j] -= row[j] * log(row[j]);
            }
        }
        entropy[j] = (1.0 / log(n_alternatives)) * entropy[j];
    }

    // Calculate divergence coefficient and weights
    std::vector<double> weights(n_criteria, 0.0);
    double sum_diff_coeff = std::accumulate(entropy.begin(), entropy.end(), 0.0, [](double acc, double e) {
        return acc + (1.0 - e);
        });
    for (size_t j = 0; j < n_criteria; ++j) {
        weights[j] = (1.0 - entropy[j]) / sum_diff_coeff;
    }

    return weights;
}

std::vector<double> topsis(const std::vector<std::vector<double>>& matrix, const std::vector<double>& weights) {
    size_t n_criteria = weights.size();

    // Weighted normalized decision matrix
    std::vector<std::vector<double>> weighted_matrix;
    for (const auto& row : matrix) {
        std::vector<double> weighted_row;
        for (size_t i = 0; i < row.size(); ++i) {
            weighted_row.push_back(row[i] * weights[i]);
        }
        weighted_matrix.push_back(weighted_row);
    }

    // Identify positive and negative ideal solutions
    std::vector<double> positive_ideal(n_criteria, std::numeric_limits<double>::lowest());
    std::vector<double> negative_ideal(n_criteria, std::numeric_limits<double>::max());

    for (size_t j = 0; j < n_criteria; ++j) {
        for (const auto& row : weighted_matrix) {
            positive_ideal[j] = std::max(positive_ideal[j], row[j]);
            negative_ideal[j] = std::min(negative_ideal[j], row[j]);
        }
    }

    // Calculate distances and relative closeness
    std::vector<double> closeness(matrix.size(), 0.0);
    for (size_t i = 0; i < matrix.size(); ++i) {
        double distance_positive = 0.0;
        double distance_negative = 0.0;

        for (size_t j = 0; j < n_criteria; ++j) {
            distance_positive += pow(weighted_matrix[i][j] - positive_ideal[j], 2);
            distance_negative += pow(weighted_matrix[i][j] - negative_ideal[j], 2);
        }

        distance_positive = sqrt(distance_positive);
        distance_negative = sqrt(distance_negative);

        closeness[i] = distance_negative / (distance_positive + distance_negative);
    }

    return closeness;
}

std::vector<double> normalize(const std::vector<double>& vec) {
    double magnitude = sqrt(std::accumulate(vec.begin(), vec.end(), 0.0, [](double acc, double x) {
        return acc + pow(x, 2);
        }));
    std::vector<double> normalized_vec;
    for (double x : vec) {
        normalized_vec.push_back(x / magnitude);
    }
    return normalized_vec;
}

double insufficient_amount(const std::vector<std::vector<double>>& matrix, double supply) {
    std::vector<double> weights = entropy_weight(matrix);
    std::vector<double> closeness = topsis(matrix, weights);
    std::vector<double> normalized_closeness = normalize(closeness);

    double insufficient_amount = 0.0;

    for (size_t i = 0; i < matrix.size(); ++i) {
        double need_sum = std::accumulate(matrix[i].begin(), matrix[i].end(), 0.0);
        insufficient_amount += need_sum - supply * normalized_closeness[i];
    }

    return insufficient_amount;
}

int main() {
    double supply = 10000.0;
    std::vector<std::vector<double>> matrix = {
        {626.0, 2560.0, 1.13},
        {2780.0, 5130.0, 1.65},
        {46.8, 7250.0, 2.21},
        {78.3, 1320.0, 5.45},
        {735.0, 7690.0, 79.9}
    };



    double insuff_amount = insufficient_amount(matrix, supply);

    for (double save_ratio = 1; save_ratio > 0.05; save_ratio -= 0.1) {
        std::vector<std::vector<double>> saved_matrix;

        for (size_t i = 0; i < matrix.size(); ++i) {
            std::vector<double> saved_row = matrix[i];
            saved_row[1] *= save_ratio;
            saved_matrix.push_back(saved_row);
        }

        std::vector<double> weights = entropy_weight(saved_matrix);
        std::vector<double> closeness = topsis(saved_matrix, weights);
        std::vector<double> normalized_closeness = normalize(closeness);

        std::cout << "节省比例：" << save_ratio << std::endl;
        std::cout << std::endl << "缺量: " << insuff_amount << std::endl;
    }



    return 0;
}