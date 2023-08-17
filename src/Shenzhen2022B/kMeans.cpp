#include "kMeans.h"

void kMeans(std::vector<User>& user_v, std::vector<Point>& center_v, std::vector<std::vector<User>>& result_v2, const std::size_t clusters, const std::size_t iterations)
{
	for (User& u : user_v)
	{
		std::vector<double> temp(clusters, 1.0);
		temp.swap(u.weight);
	}
	std::vector<Point> temp_v(clusters);
	temp_v.swap(center_v);

	// 生成随机序列供初始聚类中心使用
	static std::random_device rd;
	static XoshiroCpp::Xoroshiro128Plus gen(rd());
	std::uniform_int_distribution<std::size_t> dist(0, user_v.size() - 1);
	assert(user_v.size() > clusters);

	std::vector<size_t> rand_v;
	rand_v.reserve(clusters);
	while (rand_v.size() < clusters) {
		std::size_t num = dist(gen);
		if (std::find(rand_v.begin(), rand_v.end(), num) == rand_v.end()) {
			rand_v.push_back(num);
#ifdef _DEBUG
			std::cout << "random int:" << num << '\n';
#endif
		}
	}


	//聚类主算法

	// 各个聚类的 vector
	std::vector<std::vector<User>> cluster_v2(clusters);

	// 随机选择初始聚类中心
	std::transform(rand_v.cbegin(), rand_v.cend(), center_v.begin(),
		[&user_v](int index)->Point
		{
			assert(user_v.at(index).pos.at(0) != 0 || user_v.at(index).pos.at(1) != 0);
			return user_v.at(index).pos;
		});
#ifdef _DEBUG
	std::cout << "Initial center position: ";
	for (auto& i : center_v)
	{
		std::cout << '[' << i << "] ";
	}
	std::cout << std::endl;
#endif

	// 聚类主循环
	for (int n = 0; n < iterations; ++n) {
		std::vector<std::vector<User>> temp_v2(clusters);
		temp_v2.swap(cluster_v2);


		// 对每一个用户进行判断
		for (User& u : user_v)
		{
			// 计算用户到各个中心点的距离
			std::vector<double> distance_a(clusters);
			for (std::size_t i = 0; i < clusters; ++i)
			{
				distance_a[i] = u.weight[i] * calculateDistance(u.pos, center_v.at(i));
			}

			// 取最小的两个距离
			Point minDistance_a;
			std::partial_sort_copy(distance_a.begin(), distance_a.end(), minDistance_a.begin(), minDistance_a.end());

			// 根据距离判断接受第二近中心点的概率
			double rand = XoshiroCpp::DoubleFromBits(gen());
			assert(0 <= rand && rand <= 1);
			if (rand > minDistance_a.at(0) / minDistance_a.at(1) / n)
			{
				// 接受最近中心点
				std::size_t index = std::find(distance_a.cbegin(), distance_a.cend(), minDistance_a.at(0)) - distance_a.cbegin();
				assert(index <= clusters);
				cluster_v2.at(index).push_back(u);
			}
			else
			{
				// 接受第二中心点
#ifdef _DEBUG
				std::cout << "accept second near center: random number is " << rand << "\n";
#endif
				std::size_t index = std::find(distance_a.cbegin(), distance_a.cend(), minDistance_a.at(1)) - distance_a.cbegin();
				assert(index <= clusters);
				cluster_v2.at(index).push_back(u);
			}
		}



		for (User& u : user_v)
		{
			std::vector<double> temp(clusters, 1.0);
			temp.swap(u.weight);
		}

		for (std::size_t i = 0; i < clusters; ++i)
		{
			std::size_t size = cluster_v2.at(i).size();
			// 计算下一次迭代的距离权重
			if (size > 2 && size < 6)
			{
				for (User& u : cluster_v2.at(i))
				{
					auto userIter = find(user_v.begin(), user_v.end(), u);
					(*userIter).weight.at(i) = (A_PRICE + SWITCH_PRICE / (1.0 + 2.0 * size / user_v.size())) / B_PRICE;
				}
			}

			// 重新计算各个中心点
			center_v.at(i) = { 0.0, 0.0 };
			double& x = center_v.at(i).at(0);
			double& y = center_v.at(i).at(1);
			for (User& u2 : cluster_v2.at(i))
			{
				x += u2.pos.at(0);
				y += u2.pos.at(1);
			}
			if (size != 0)
			{
				x /= size;
				y /= size;
			}
			assert(0 <= x);
			assert(0 <= y);
		}
#ifdef _DEBUG
		std::cout << "Center location for iteration " << n << " :\n";
		for (auto& i : center_v)
		{
			std::cout << '[' << i << "] ";
		}
		std::cout << std::endl;
#endif
	}

	cluster_v2.swap(result_v2);
}