
#include "pathfinding.h"

#include <iostream>
#include <string>
#include <queue>




namespace sc2 {

std::vector<Node*> Graph::ReconstructPath(std::map<Node*, Node*> came_from, Node* end)
{
	std::vector<Node*> path_r;
	Node* current = end;
	while (current != nullptr)
	{
		path_r.push_back(current);
		current = came_from[current];
	}
	std::vector<Node*> path;
	for (int i = (int)path_r.size() - 1; i >= 0; i--)
	{
		path.push_back(path_r[i]);
	}
	return path;
}

Node* Graph::FindClosestNode(Point2D pos)
{
	Node* closest_node = nodes[0];
	float dist = Distance2D(pos, closest_node->pos);
	for (const auto &node : nodes)
	{
		if (Distance2D(pos, node->pos) < dist)
		{
			closest_node = node;
			dist = Distance2D(pos, node->pos);
		}
	}
	return closest_node;
}

std::vector<Node*> Graph::FindPath(Node* start, Node* end)
{
	std::priority_queue<QNode, std::vector<QNode>, NodeCompare> heap;
	std::map<Node*, Node*> came_from;
	std::map<Node*, float> dist_to;
	std::vector<Node*> visited;
	

	heap.push(QNode(start, Distance2D(start->pos, end->pos)));
	dist_to[start] = 0;
	came_from[start] = nullptr;

	while (!heap.empty())
	{
		QNode current = heap.top();
		heap.pop();
		if (std::find(visited.begin(), visited.end(), current.node) != visited.end())
			continue;
		if (current.node == end)
			return ReconstructPath(came_from, end);


		for (const auto &node : current.node->connections)
		{
			float dist_to_node = dist_to[current.node] + Distance2D(current.node->pos, node->pos);
			if (dist_to.count(node) == 0 || dist_to_node < dist_to[node])
			{
				QNode new_node = QNode(node, dist_to_node + Distance2D(node->pos, end->pos));
				heap.push(new_node);
				dist_to[node] = dist_to_node;
				came_from[node] = current.node;
			}
		}

		visited.push_back(current.node);
	}
	// Error
	return {};
}

}