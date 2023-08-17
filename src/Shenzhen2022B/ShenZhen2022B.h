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
static constexpr double MAIN_PRICE = 325.7;
static constexpr double A_PRICE = 188.6;
static constexpr double B_PRICE = 239.4;
static constexpr double SWITCH_PRICE = 56.8;
static constexpr double SUB_SWITCH_PRICE = 2.6;
static constexpr double SCALER = 0.001;

using Point = std::array<double, 2>;
struct User
{
	std::vector<double> weight;
	Point pos = { 0, 0 };
	int id = -1;

	User() = default;
	User(const User& ori) = default;
	User& operator=(const User& ori) = default;
	User(User&& ori) = default;
	User& operator=(User&& ori) = default;
	explicit User(const Point& position, int positive_id) : pos(position), id(positive_id) {}
};

class Node {
private:
	const std::shared_ptr<User> m_user_p = nullptr;
	std::shared_ptr<Node> m_parent_p = nullptr;
	std::vector<std::shared_ptr<Node>> m_child_p_v{};
	std::size_t m_clusterID = 0;
	std::size_t m_load = 0;
	float m_reliability = 1.0;

public:
	Node() = default;
	explicit Node(const std::shared_ptr<User> u) : m_user_p(u) {}
	explicit Node(const User& u) : m_user_p(std::make_shared<User>(u)) {}
	Node(const User& u, std::size_t cluster_id)
		: m_user_p(std::make_shared<User>(u)), m_clusterID(cluster_id) {}
	Node(const std::shared_ptr<User>& u, std::size_t cluster_id)
		: m_user_p(u), m_clusterID(cluster_id) {}

	const Node& operator[](std::size_t index) const
	{
		assert(0 <= index && index < m_child_p_v.size());
		return *m_child_p_v[index];
	}

	Node& operator[](std::size_t index)
	{
		assert(0 <= index && index < m_child_p_v.size());
		return *m_child_p_v[index];
	}


	[[nodiscard]] auto
		getUser_p() -> std::shared_ptr<User> const
	{
		return m_user_p;
	}
	[[nodiscard]] auto
		getParent_p() -> std::shared_ptr<Node> const
	{
		return m_parent_p;
	}
	[[nodiscard]] constexpr auto
		getID() -> std::size_t const
	{
		return m_clusterID;
	}
	[[nodiscard]] constexpr auto
		getLoad() -> std::size_t const
	{
		return m_load;
	}
	[[nodiscard]] constexpr auto
		getReliability() -> float const
	{
		return m_reliability;
	}

	[[nodiscard]] auto
		getChild() -> std::vector<std::shared_ptr<Node>> const
	{
		return m_child_p_v;
	}


	Node& setParent(const std::shared_ptr<Node> parent)
	{
		m_parent_p = parent;
		return *this;
	}
	Node& pushChild(const std::shared_ptr<Node> child)
	{
		m_child_p_v.push_back(child);
		return *this;
	}
	Node& addLoad(const std::shared_ptr<Node>& child)
	{
		m_load += child->getLoad();
		return *this;
	}
	Node& addLoad(const std::size_t load)
	{
		m_load += load;
		return *this;
	}
	Node& pushChild(const Node& child)
	{
		return pushChild(std::make_shared<Node>(child));
	}
	Node& mulReliability(const double f)
	{
		m_reliability *= f;
		return *this;
	}
};

template <typename Predicate>
std::shared_ptr<Node> find(const std::shared_ptr<Node>& root, Predicate predicate) {
	if (predicate(root)) {
		return root;
	}

	for (const auto& child : root->getChild()) {
		auto result = find(child, predicate);
		if (result) {
			return result;
		}
	}

	return nullptr;
}



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



/**
 * @brief 计算两个二维点之间的欧几里德距离。
 *
 * 给定两个二维点 p1 和 p2，该函数计算并返回它们之间的欧几里德距离。欧几里德距离是两点之间的直线距离。
 *
 * @param p1 第一个二维点的坐标 {x, y}。
 * @param p2 第二个二维点的坐标 {x, y}。
 * @return 两点之间的欧几里德距离。
 */
[[nodiscard]] inline double calculateDistance(const Point& p1, const Point& p2)
{
	double dx = p1[0] - p2[0];
	double dy = p1[1] - p2[1];
	return std::sqrt(dx * dx + dy * dy);
}

/**
 * @brief 计算两个用户之间的欧几里德距离。
 *
 * 给定两个用户 u1 和 u2，该函数计算并返回它们之间的欧几里德距离。欧几里德距离是两点之间的直线距离。
 * 此函数使用用户的位置信息来计算距离。
 *
 * @param u1 第一个用户。
 * @param u2 第二个用户。
 * @return 两个用户之间的欧几里德距离。
 */
[[nodiscard]] inline double calculateDistance(const User& u1, const User& u2)
{
	double dx = u1.pos[0] - u2.pos[0];
	double dy = u1.pos[1] - u2.pos[1];
	return std::sqrt(dx * dx + dy * dy);
}

/**
 * @brief 构造路线函数
 *
 * 随机选择初始点聚类，并使用 MST 相关算法实现最短路构造。
 *
 * @param user_v 所有用户
 * @param clusters 期望的聚类数（可能会小于该数，结果储存于此）
 * @param log 是否使用 std::cout 输出中间步骤
 *
 * @result 指向根节点
 */
std::shared_ptr<Node> buildRoute(std::vector<User>& user_v, std::size_t& clusters, bool log = true);

/**
 * @brief 计算各节点的负载
 *
 * 递归子节点个数
 *
 * @param root 根节点入口
 *
 */
void calculateLoad(const std::shared_ptr<Node>& root);

/**
 * @brief 计算总价格
 *
 * 使用递归计算所有路线和开关价格总和
 *
 * @param root 根节点入口
 * @param clusters 聚类个数
 *
 * @result 返回总价的值
 */
double calculateCost(const std::shared_ptr<Node>& root);


/**
 * @brief 计算可靠性
 *
 * 使用递推计算每一个点的可靠性
 *
 * @param root 根节点入口
 *
 */
void calculateReliability(const std::shared_ptr<Node>& root);

/**
 * @brief 查找最低可靠性
 *
 * @param root 根节点入口
 *
 *
 */


