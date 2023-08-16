#pragma once
#include "ShenZhen2022B.h"

std::shared_ptr<TreeNode> buildBinaryTree(const std::vector<User>& users, const std::vector<const User*>& remainingNodes);
const User* findNearestUser(const User& user, const std::vector<const User*>& nodes);
std::shared_ptr<TreeNode> balanceBinaryTree(const std::shared_ptr<TreeNode> node);
void printTree(const std::shared_ptr<TreeNode> node, int level = 0, bool isLeft = true);
