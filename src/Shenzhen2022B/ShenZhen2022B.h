#pragma once
#include <iostream>
#include <algorithm>
#include <random>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <numeric>
#include <iterator>
#include <execution>
#include <queue>
#include <unordered_map>
#include "../../deps/Xoshiro-cpp/XoshiroCpp.hpp"
#include "../tools/data_processing.h"

// 常量定义
static constexpr int CLUSTER_NUM = 7;
static constexpr int ITERATIONS = 100;
static constexpr double A_PRICE = 1.269353;
static constexpr double B_PRICE = 188.6;
static constexpr double SWITCH_PRICE = 56.8;

using Point = std::array<double, 2>;
struct User
{
	std::vector<double> weight;
	Point pos = { 0, 0 };
	double watt;
	int id = -1;

	User() = default;
	User(const Point& position, int positive_id) : pos(position), id(positive_id) {}
};

struct TreeNode {
	const User* user;
	std::shared_ptr<TreeNode> left = nullptr;
	std::shared_ptr<TreeNode> right = nullptr;
	std::shared_ptr<TreeNode>

		TreeNode(const User* u) : user(u) {}
};

inline std::istream& operator>>(std::istream& is, User& user)
{

	is >> user.id >> user.pos.at(0) >> user.pos.at(1);
	return is;
}
inline std::ostream& operator<<(std::ostream& os, const Point& pos)
{
	os << pos.at(0) << ", " << pos.at(1);
	return os;
}
[[nodiscard]] inline bool operator==(const User& lhs, const User& rhs) { return lhs.id == rhs.id; }

/* 名称：两点间距离辅助函数
*  输入：
*	1. 起始点坐标
*	2. 终点坐标
*  输出：两点距离
*/
[[nodiscard]] inline double calculateDistance(const Point& p1, const Point& p2)
{
	double dx = p1[0] - p2[0];
	double dy = p1[1] - p2[1];
	return std::sqrt(dx * dx + dy * dy);
}



