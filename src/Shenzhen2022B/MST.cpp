#include "MST.h"
#include <execution>
#include <set>

// 弃用的主路线构造函数
void connectMain(const Point& target, const std::vector<Point>& nodes, std::vector<Point>& result)
{
	std::vector<User> connectedNodes;

	// Connect each node to the target point and add to the vector
	for (std::size_t i = 0; i < nodes.size(); ++i)
	{
		User user{ nodes[i], -(static_cast<int>(i) + 1) };
		connectedNodes.push_back(user);
	}

	// Sort the connected nodes based on their distance to the target
	std::sort(connectedNodes.begin(), connectedNodes.end(),
		[&target](const User& a, const User& b) {
			return calculateDistance(a.pos, target) < calculateDistance(b.pos, target);
		});

	std::vector<Point> temp;
	for (auto& u : connectedNodes)
		temp.emplace_back(std::move(u.pos));

	temp.insert(temp.begin(), target);
	temp.swap(result);
}

// 主线构造函数
void constructMainLine(const std::shared_ptr<Node>& powerNode, std::vector<std::shared_ptr<Node>>& nodeList)
{
	// 当前节点及其位置
	std::shared_ptr<Node> nowNode = powerNode;
	Point nowPos = powerNode->getUser_p()->pos;

	while (!nodeList.empty())
	{
		// 找出离现有节点距离最小的节点
		std::nth_element(nodeList.begin(), nodeList.end() - 1, nodeList.end(),
			[&nowPos](const std::shared_ptr<Node> lhs, const std::shared_ptr<Node> rhs) -> bool
			{
				return calculateDistance(nowPos, lhs->getUser_p()->pos) > calculateDistance(nowPos, rhs->getUser_p()->pos);
			});

		auto& thisNode = nodeList.back();
		Point& thisPos = thisNode->getUser_p()->pos;

		if (
			calculateDistance(nowPos, thisPos) <=
			calculateDistance(powerNode->getUser_p()->pos, thisPos)
			)
		{
			thisNode->setParent(nowNode);
			nowNode->pushChild(thisNode);
		}
		else
		{
			thisNode->setParent(powerNode);
			powerNode->pushChild(thisNode);
		}

		// 更新为新节点
		nowNode = thisNode;
		nowPos = thisPos;
		nodeList.pop_back();
	}
}


// 弃用的构造支线函数 - Kruskal
std::size_t find_set(std::size_t x, std::vector<std::size_t>& s)
{
	if (x != s[x])
		s[x] = find_set(s[x], s);
	return s[x];
}
struct Edge { double w; std::size_t u, v; };
void constructBranch_Kruskal(const std::shared_ptr<Node> start, std::vector<std::shared_ptr<Node>>& userList)
{
	userList.push_back(start);
	// 计算所有边的长度
	std::vector<Edge> edges;
	for (std::size_t i = 0; i < userList.size(); ++i)
	{
		for (std::size_t j = 0; i < userList.size(); ++i)
		{
			if (i == j)
				continue;

			edges.push_back(Edge{
				calculateDistance(userList.at(i)->getUser_p()->pos, userList.at(j)->getUser_p()->pos),
					i,
					j });
		}
	}

	// sort the edges
	std::sort(edges.begin(), edges.end(), [](Edge& a, Edge& b) -> bool
		{
			return a.w < b.w;
		});

	// 并查集初始化
	std::vector<std::size_t> s(userList.size());
	for (std::size_t i = 0; i < userList.size(); ++i)
		s.at(i) = i;

	std::size_t count = 0;
	for (std::size_t i = 0; i < userList.size(); ++i)
	{
		if (count == userList.size() - 1) break;

		std::size_t e1 = find_set(edges.at(i).u, s);
		std::size_t e2 = find_set(edges.at(i).v, s);
		if (e1 == e2)
			continue;

		// 无法实现，这个算法要求无向图
	}

}

// 支线构造函数 - Prim
void constructBranch(const std::shared_ptr<Node> start, std::vector<std::shared_ptr<Node>>& userList)
{
	std::vector<std::shared_ptr<Node>> u{ start };
	while (!userList.empty())
	{
		// 这个排序的计算量非常大，可以优化
		std::sort(userList.begin(), userList.end(),
			[&u](std::shared_ptr<Node>& a, std::shared_ptr<Node>& b) -> bool
			{
				double minDistance_a = DBL_MAX, minDistance_b = DBL_MAX;
				for (auto& i : u)
				{
					double dis_a = calculateDistance(a->getUser_p()->pos, i->getUser_p()->pos);
					double dis_b = calculateDistance(b->getUser_p()->pos, i->getUser_p()->pos);
					if (dis_a < minDistance_a)
						minDistance_a = dis_a;
					if (dis_b < minDistance_b)
						minDistance_b = dis_b;
				}
				return minDistance_a > minDistance_b;
			});

		std::shared_ptr<Node> thisNode = userList.back();
		std::shared_ptr<Node> parentNode = nullptr;
		double minDistance = DBL_MAX;
		for (auto& i : u)
		{
			double dis = calculateDistance(thisNode->getUser_p()->pos, i->getUser_p()->pos);
			if (dis < minDistance)
			{
				parentNode = i;
				minDistance = dis;
			}
		}

		parentNode->pushChild(thisNode);
		thisNode->setParent(parentNode);
		u.emplace_back(thisNode);
		userList.pop_back();
	}
}



// 输出树结构
void printTreeDataHelper(const std::shared_ptr<Node>& node, int depth = 0)
{
	if (!node)
		return;

	const std::shared_ptr<User>& user = node->getUser_p();
	const Point& pos = user->pos;
	int id = user->id;

	for (int i = 0; i < depth; ++i)
		std::cout << "  ";
	std::cout << "ID " << id << ", Position (" << pos[0] << ", " << pos[1] << "), " << " Load " << node->getLoad() << ", Reliability " << node->getReliability() << "', ...\n'";

	for (const auto& child : node->getChild())
	{
		printTreeDataHelper(child, depth + 1);
	}
}

void printTreeData(const std::shared_ptr<Node>& root)
{
	if (!root)
		return;

	std::cout << "'";
	printTreeDataHelper(root);
}

void printTreeHelper(const std::shared_ptr<Node>& node, int depth = 0)
{
	if (!node)
		return;

	const std::shared_ptr<User>& user = node->getUser_p();
	const Point& pos = user->pos;
	int id = user->id;

	for (int i = 0; i < depth; ++i)
		std::cout << "  ";
	std::cout << "|ID " << id << ", Position (" << pos[0] << ", " << pos[1] << ")," << ", Load " << node->getLoad() << " Reliability " << node->getReliability() << std::endl;

	for (const auto& child : node->getChild())
	{
		printTreeHelper(child, depth + 1);
	}
}

void printTree(const std::shared_ptr<Node>& root)
{
	if (!root)
		return;

	printTreeHelper(root);
}

void flattenNode(const std::shared_ptr<Node>& root, std::vector<Node>& result)
{
	if (!root)
		return;

	std::vector<Node>().swap(result);
	result.emplace_back(*root);

	const std::vector<std::shared_ptr<Node>>& children = root->getChild();
	for (const std::shared_ptr<Node>& child : children) {
		flattenNode(child, result);
	}
}

void flattenLine(const std::shared_ptr<Node>& root, std::vector<std::pair<Node, Node>>& result) {
	if (!root)
		return;

	std::vector<std::pair<Node, Node>>().swap(result);
	const std::vector<std::shared_ptr<Node>>& children = root->getChild();
	for (const std::shared_ptr<Node>& child : children) {
		result.emplace_back(*child, *root);
		flattenLine(child, result);
	}
}