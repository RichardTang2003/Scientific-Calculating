#include "binaryTree.h"
#include <algorithm>
#include <cmath>
#include <iostream>



const std::shared_ptr<User> findNearestUser(const User& user, const std::vector<const std::shared_ptr<User>>& nodes)
{
	auto compareDistance = [&user](const User* a, const User* b) {
		return calculateDistance(user.pos, a->pos) < calculateDistance(user.pos, b->pos);
		};

	auto nearestIt = std::min_element(nodes.begin(), nodes.end(), compareDistance);

	if (nearestIt != nodes.end()) {
		return std::make_shared<User>(*nearestIt);
	}

	return nullptr;
}



std::shared_ptr<TreeNode> buildBinaryTree(const std::vector<User>& users, const std::vector<const User*>& remainingNodes)
{
	if (remainingNodes.empty()) {
		return nullptr;
	}

	const User* nearestUser = findNearestUser(*remainingNodes[0], remainingNodes);
	std::shared_ptr<TreeNode> node = std::make_shared<TreeNode>(nearestUser);

	std::vector<const User*> leftNodes;
	std::vector<const User*> rightNodes;
	for (const User* u : remainingNodes) {
		if (u != nearestUser) {
			(calculateDistance(u->pos, nearestUser->pos) < calculateDistance(u->pos, node->user->pos)) ?
				leftNodes.push_back(u) : rightNodes.push_back(u);
		}
	}

	node->left = buildBinaryTree(users, leftNodes);
	node->right = buildBinaryTree(users, rightNodes);

	return node;
}

void traverseInOrder(const std::shared_ptr<TreeNode> node, std::vector<const std::shared_ptr<User>>& sortedNodes)
{
	if (!node) {
		return;
	}

	traverseInOrder(node->left, sortedNodes);
	sortedNodes.push_back(node->user);
	traverseInOrder(node->right, sortedNodes);
}

std::shared_ptr<TreeNode> buildBalancedTree(const std::vector<const std::shared_ptr<User>>& nodes, int start, int end)
{
	if (start > end) {
		return nullptr;
	}

	int mid = (start + end) / 2;
	std::shared_ptr<TreeNode> node = std::make_shared<TreeNode>(nodes[mid]);
	node->left = buildBalancedTree(nodes, start, mid - 1);
	node->right = buildBalancedTree(nodes, mid + 1, end);

	return node;
}


std::shared_ptr<TreeNode> balanceBinaryTree(const std::shared_ptr<TreeNode> node)
{
	if (!node) {
		return nullptr;
	}

	// 构建一个有序节点列表
	std::vector<const std::shared_ptr<User>> sortedNodes;
	traverseInOrder(node, sortedNodes);

	// 根据有序列表构建平衡二叉树
	return buildBalancedTree(sortedNodes, 0, sortedNodes.size() - 1);
}

void printTree(const std::shared_ptr<TreeNode> node, int level, bool isLeft)
{
	if (!node) {
		return;
	}

	if (level > 0) {
		std::cout << std::string((level - 1) * 4, ' ');
		if (isLeft) {
			std::cout << " ├─L ";
		}
		else {
			std::cout << " ├─R ";
		}
	}

	std::cout << "User " << node->user->id << " at position (" << node->user->pos[0] << ", " << node->user->pos[1] << ")" << std::endl;

	printTree(node->left, level + 1, true);
	printTree(node->right, level + 1, false);
}

