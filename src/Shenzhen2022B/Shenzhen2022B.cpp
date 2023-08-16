#include "ShenZhen2022B.h"
#include "kMeans.h"
#include "MST.h"
#include "binaryTree.h"
#include <thread>
void one(std::vector<User>& user_v);

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

	one(user_v);
	
}



void one(std::vector<User>& user_v)
{


	// 对用户进行聚类分析
	std::vector<Point> center_v;
	std::vector<std::vector<User>> result_v2;
	kMeans(user_v, center_v, result_v2);
	// 输出聚类结果
	std::cout << "========= CLUSTER RESULT ==========\n";
	for (std::size_t i = 0; i < center_v.size(); ++i)
	{
		std::cout << '[' << center_v.at(i) << "] ";
		for (auto& e : result_v2.at(i))
		{
			std::cout << e.id << ' ';
		}
		std::cout << std::endl;
	}

	// 构造主线
	std::shared_ptr<TreeNode> mainLine;
	center_v.erase(std::remove(center_v.begin(), center_v.end(), Point{ 0, 0 }), center_v.end());
	result_v2.erase(std::remove(result_v2.begin(), result_v2.end(), std::vector<User>{}), result_v2.end());
	connectMain({ 898.402569999998, 1007.05848000000 }, center_v, mainLine);
	// 输出主线坐标
	std::cout << "========= MAINLINE RESULT ==========\n";
	printTree(mainLine);

	// 构造支线
	std::vector<std::shared_ptr<TreeNode>> tree_p_v;
	for (std::size_t i = 0; i < center_v.size(); ++i)
	{
		std::vector<const User*> userPointers;
		for (const User& user : result_v2[i]) {
			userPointers.push_back(&user);
		}

		// 构造二叉树
		auto tree_ptr = buildBinaryTree(result_v2[i], userPointers);
		auto balancedTree_ptr = balanceBinaryTree(tree_ptr);
		tree_p_v.emplace_back(balancedTree_ptr);
	}
	// 输出支线二叉树
	std::cout << "========= TREE RESULT ==========\n";
	for (std::size_t i = 0; i < center_v.size(); ++i)
	{
		std::cout << "Tree for center " << '[' << center_v.at(i) << "]: \n";
		printTree(tree_p_v.at(i));
		std::cout << std::endl;
	}


	std::cout << "Finished!" << std::endl;
#ifndef _DEBUG
	std::cin.get();
	std::cin.get();
#endif
}

