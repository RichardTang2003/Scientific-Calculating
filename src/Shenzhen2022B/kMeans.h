#pragma once
#include "ShenZhen2022B.h"
/**
* @brief 加权 K-Means 算法
*
* @param user_v 用户数据
* @param center_a 用于储存聚类的中点信息
* @param result_v2 用于储存各个簇包含的用户信息
* @param cluster 聚类的簇个数
*
* @note 参数2与3的信息依 index 对应
* @note 注意！User 结构中的 weight 会被改变，请提前备份！
*/
void kMeans(std::vector<User>& user_v, std::vector<Point>& center_v, std::vector<std::vector<User>>& result_v2, const std::size_t clusters, const std::size_t iterations);