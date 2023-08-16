#include "MST.h"

void connectMain(const Point& target, const std::vector<Point>& nodes, std::vector<Point>& result)
{
	std::vector<User> connectedNodes;

	// Connect each node to the target point and add to the vector
	for (int i = 0; i < nodes.size(); ++i)
	{
		User user = { nodes[i], -(i + 1) };
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

void connectMain(const Point& target, const std::vector<Point>& nodes, std::shared_ptr<TreeNode>& result)
{
    std::vector<User> connectedNodes;

    // Connect each node to the target point and add to the vector
    for (int i = 0; i < nodes.size(); ++i)
    {
        User user = { nodes[i], -(i + 1) };
        connectedNodes.push_back(user);
    }

    // Sort the connected nodes based on their distance to the target
    std::sort(connectedNodes.begin(), connectedNodes.end(),
        [&target](const User& a, const User& b) {
            return calculateDistance(a.pos, target) < calculateDistance(b.pos, target);
        });

    std::vector<std::shared_ptr<TreeNode>> treeNodes;

    for (const auto& u : connectedNodes) {
        auto node = std::make_shared<TreeNode>(&u);
        if (!treeNodes.empty()) {
            treeNodes.back()->right = node;
        }
        treeNodes.push_back(node);
    }

    result = treeNodes.empty() ? nullptr : treeNodes[0];
}