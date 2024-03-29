#pragma once
#include "ShenZhen2022B.h"

/**
* @brief 最小生成树构造算法
*
* 已弃用
*
* @param target 中心点坐标（起始点坐标）
* @param nodes 需要连接点的坐标数组
* @param result 用于储存结果的 Point Vector
*
*/
[[deprecated]]
void connectMain(const Point& target, const std::vector<Point>& nodes, std::vector<Point>& result);




/**
* @brief 主路线构造函数
*
* @param power_p 电源节点
* @param pointList 需要排序的中心节点
*
* @note 结果将储存在电源节点 Node 中，以指针的方式构建树结构
*/
void constructMainLine(const std::shared_ptr<Node>& powerNode, std::vector<std::shared_ptr<Node>>& nodeList);

/**
 * @brief 使用Prim算法构建分支，连接用户形成树状结构。
 *
 * 此函数使用Prim算法构建分支，将用户连接到指定起始节点，形成最小生成树的分支。
 * 结果的树形结构连接所有用户，并且是一个生成树。
 *
 * @param start 指向分支起始节点的共享指针。
 * @param userList 用户指针向量。每个用户必须具有有效的位置（pos）和唯一的正整数ID。
 *
 * @note 在算法执行过程中，将修改 userList 向量以存储附加信息。
 */
void constructBranch(const std::shared_ptr<Node> start, std::vector<std::shared_ptr<Node>>& userList);

/**
 * @brief 递归地打印以给定节点为根的树的内容。
 *
 * 该函数使用深度优先遍历递归地打印以指定节点为根的树的内容。每个节点的 ID、位置和子节点都会显示，
 * 并通过适当的缩进来可视化树的结构。
 *
 * @param node 指向要打印的树的根节点的 shared_ptr。
 * @param depth 遍历的当前深度（用于缩进）。
 */
void printTree(const std::shared_ptr<Node>& root);

/**
 * @brief 以MATLAB容易使用的格式打印以给定节点为根的树的内容。
 *
 * 该函数使用深度优先遍历递归地打印以指定节点为根的树的内容。每个节点的 ID、位置和子节点都会显示，
 * 并通过适当的缩进来可视化树的结构。
 *
 * @param node 指向要打印的树的根节点的 shared_ptr。
 * @param depth 遍历的当前深度（用于缩进）。
 */
void printTreeData(const std::shared_ptr<Node>& root);

/**
 * @brief 将树中的所有节点存放到指定的向量中（展开树）。
 *
 * 此函数递归地将给定根节点为起点的树中的所有节点展开，并存储到指定的向量中。
 *
 * @param root 要展开的树的根节点的指针。
 * @param result 存储节点的向量，用于存放展开后的节点。
 */
void flattenNode(const std::shared_ptr<Node>& root, std::vector<Node>& result);


/**
 * @brief 将树中的所有连线存放到指定的向量中。
 *
 * 此函数递归地将给定根节点为起点的树中的所有节点以线性关系展开，
 * 并将每个节点与其父节点（如果存在）存储为一对存放到指定的向量中。
 *
 * @param root 要展开的树的根节点的指针。
 * @param result 存储节点和父节点之间线性关系的向量，存放为一对（Node, Node）。
 */
void flattenLine(const std::shared_ptr<Node>& root, std::vector<std::pair<Node, Node>>& result);
