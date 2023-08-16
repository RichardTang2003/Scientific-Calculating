#pragma once
#include "ShenZhen2022B.h"
/* 名称：加权 K-Means 算法
*  输入：
*	1. 用户数据  注意！User 结构中的 weight 会被改变，请提前备份！
*	2. 用于储存聚类的中点信息
*	3. 用于储存各个簇包含的用户信息
*	4. 聚类的簇个数
*  输出： 无
*
*  说明：参数2与3的信息依 index 对应
*/
void kMeans(std::vector<User>& user_v, std::vector<Point>& center_a, std::vector<std::vector<User>>& result_v2, const int clusters = CLUSTER_NUM);