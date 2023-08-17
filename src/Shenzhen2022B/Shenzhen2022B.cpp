#include "ShenZhen2022B.h"
#include "kMeans.h"
#include "MST.h"
#include <thread>

int main()
{
	// 读取数据
#ifdef _DEBUG
	std::ifstream infile("data/user.txt");
#endif
#ifndef _DEBUG
	std::ifstream infile("user.txt");
	if (!infile)
	{
		std::string filename;
		std::cerr << "Enter your data file name:" << std::endl;
		while (std::cin >> filename) {
			infile.open(filename);
			if (infile) {
				break;
			}

			std::cerr << "Unable to open " << filename << "\nTry again:" << std::endl;
		}
	}
#endif

	std::vector<User> user_v;
	user_v.reserve(30);
	while (infile) {
		User temp;
		infile >> temp;
		if (temp.id != -1)
			user_v.push_back(temp);
	}

	std::size_t clusters = 7;
	auto root = buildRoute(user_v, clusters);
	calculateLoad(root);
	calculateReliability(root);
	double cost = calculateCost(root);


	std::cout << "============ Final RESULT ===========\n";
	printTreeData(root);
	std::cout << "Finished! Total Cost: " << cost << std::endl;

#ifndef _DEBUG
	std::cin.get();
	std::cin.get();
#endif
}


std::shared_ptr<Node> buildRoute(std::vector<User>& user_v, std::size_t& clusters, bool log)
{
	// 对用户进行聚类分析
	std::vector<Point> center_v;
	std::vector<std::vector<User>> cluster_v2;
	kMeans(user_v, center_v, cluster_v2, clusters, 100);
	// 输出聚类结果
	if (log)
	{
		std::cout << "========= CLUSTER RESULT ==========\n";
		for (std::size_t i = 0; i < center_v.size(); ++i)
		{
			std::cout << '[' << center_v.at(i) << "] ";
			for (auto& e : cluster_v2.at(i))
			{
				std::cout << e.id << ' ';
			}
			std::cout << std::endl;
		}
	}

	// 去除空聚类并准备节点
	center_v.erase(std::remove(center_v.begin(), center_v.end(), Point{ 0, 0 }), center_v.end());
	cluster_v2.erase(std::remove(cluster_v2.begin(), cluster_v2.end(), std::vector<User>{}), cluster_v2.end());
	assert(center_v.size() == cluster_v2.size());

	std::vector<std::shared_ptr<Node>> centerNodes;
	std::vector<std::vector<std::shared_ptr<Node>>> userNodes_v;
	clusters = center_v.size();
	userNodes_v.resize(clusters);
	for (std::size_t i = 0; i < clusters; ++i)
	{
		centerNodes.emplace_back<std::shared_ptr<Node>>(std::make_shared<Node>(User(center_v.at(i), -((int)i + 2)), i + 1));
		for (std::size_t j = 0; j < cluster_v2.at(i).size(); ++j)
		{
			userNodes_v.at(i).emplace_back<std::shared_ptr<Node>>(std::make_shared<Node>(cluster_v2.at(i).at(j), i + 1));
		}
	}
	assert(centerNodes.size() == userNodes_v.size());

	// 电源节点
	std::shared_ptr<User> powerUser = std::make_shared<User>(Point{ 898.40256, 1007.05848 }, 0);
	std::shared_ptr<Node> powerNode = std::make_shared<Node>(powerUser);
	// 构造主线
	constructMainLine(powerNode, centerNodes);
	// 输出主线构造结果
	if (log)
	{
		std::cout << "========= MAINLINE RESULT ==========\n";
		printTreeData(powerNode);
	}


	// 构造支线
	for (std::size_t i = 0; i < clusters; ++i)
	{
		constructBranch(find(powerNode, [i](const std::shared_ptr<Node>& node) -> bool
			{
				return (node->getID() == i + 1);
			}), userNodes_v.at(i));

	}
	// 输出全局图
	if (log)
	{
		std::cout << "============ Route RESULT ===========\n";
		printTreeData(powerNode);
		std::cout << "Finished!" << std::endl;
	}

	return powerNode;
}

void calculateLoad(const std::shared_ptr<Node>& root)
{
	assert(root);
	assert(root->getUser_p()->id != -1);

	if (root->getUser_p()->id > 0)
		root->addLoad(1);

	if (root->getChild().empty())
		return;

	for (auto& child : root->getChild())
	{
		calculateLoad(child);
		root->addLoad(child);
	}

	return;
}

double calculateCost(const std::shared_ptr<Node>& root)
{
	double totalPrice = 0.0;
	auto& parent_p = root->getParent_p();
	int id = root->getUser_p()->id;



	if (parent_p != nullptr)
	{
		Point& start = parent_p->getUser_p()->pos;
		Point& end = root->getUser_p()->pos;
		double dis = SCALER * calculateDistance(start, end);
		if (id < 0)
		{
			// 主线
			totalPrice += MAIN_PRICE * dis;
			totalPrice += SWITCH_PRICE;
		}
		else
		{
			//支线
			totalPrice += SUB_SWITCH_PRICE;
			if (root->getLoad() > 2)
			{
				totalPrice += B_PRICE * dis;
			}
			else
			{
				totalPrice += A_PRICE * dis;
			}
		}
	}
	else
	{
		// 删去直连路线上的开关
		totalPrice -= SWITCH_PRICE * root->getChild().size();
	}

	if (root->getChild().empty())
		return totalPrice;

	for (auto& child : root->getChild())
	{
		totalPrice += calculateCost(child);
	}

	return totalPrice;
}

void calculateReliability(const std::shared_ptr<Node>& root)
{
	auto& parent_p = root->getParent_p();
	if (parent_p != nullptr)
	{
		Point& start = parent_p->getUser_p()->pos;
		Point& end = root->getUser_p()->pos;
		root->mulReliability(parent_p->getReliability() * 0.998 * 0.995 * (1 - 0.002 * SCALER * calculateDistance(start, end)));
	}

	for (auto& child : root->getChild())
	{
		calculateReliability(child);
	}
}

