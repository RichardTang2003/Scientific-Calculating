#pragma once
#include "ShenZhen2022B.h"

/* 名称：最小生成树构造算法
*  输入：
*	1. 中心点坐标（起始点坐标）
*	2. 需要连接点的坐标数组
*   3. 用于储存结果的 Point Vector
*  输出：无
* 
*/
void connectMain(const Point& target, const std::vector<Point>& nodes, std::vector<Point>& result);


/* 名称：最小生成树构造算法
*  输入：
*	1. 中心点坐标（起始点坐标）
*	2. 需要连接点的坐标数组
*	3. 用于储存结果的 TreeNode 起始指针
*  输出：无
*
*  说明：输出的TreeNode包含 User 结构的指针，将自动生成负数 id
*/
void connectMain(const Point& target, const std::vector<Point>& nodes, std::shared_ptr<TreeNode>& result);
