
#include "pathfinding.h"

#include <iostream>
#include <string>
#include <queue>


namespace sc2 {

std::vector<GraphNode*> Graph::ReconstructPath(std::map<GraphNode*, GraphNode*> came_from, GraphNode* end) const
{
	std::vector<GraphNode*> path_r;
	GraphNode* current = end;
	while (current != nullptr)
	{
		path_r.push_back(current);
		current = came_from[current];
	}
	std::vector<GraphNode*> path;
	for (int i = (int)path_r.size() - 1; i >= 0; i--)
	{
		path.push_back(path_r[i]);
	}
	return path;
}

GraphNode* Graph::FindClosestNode(Point2D pos) const
{
	GraphNode* closest_GraphNode = nodes[0];
	float dist = Distance2D(pos, closest_GraphNode->pos);
	for (const auto &GraphNode : nodes)
	{
		if (Distance2D(pos, GraphNode->pos) < dist)
		{
			closest_GraphNode = GraphNode;
			dist = Distance2D(pos, GraphNode->pos);
		}
	}
	return closest_GraphNode;
}

std::vector<GraphNode*> Graph::FindPath(GraphNode* start, GraphNode* end) const
{
	std::priority_queue<QGraphNode, std::vector<QGraphNode>, GraphNodeCompare> heap;
	std::map<GraphNode*, GraphNode*> came_from;
	std::map<GraphNode*, float> dist_to;
	std::vector<GraphNode*> visited;
	

	heap.push(QGraphNode(start, Distance2D(start->pos, end->pos)));
	dist_to[start] = 0;
	came_from[start] = nullptr;

	while (!heap.empty())
	{
		QGraphNode current = heap.top();
		heap.pop();
		if (std::find(visited.begin(), visited.end(), current.node) != visited.end())
			continue;
		if (current.node == end)
			return ReconstructPath(came_from, end);


		for (const auto &GraphNode : current.node->connections)
		{
			float dist_to_GraphNode = dist_to[current.node] + Distance2D(current.node->pos, GraphNode->pos);
			if (dist_to.count(GraphNode) == 0 || dist_to_GraphNode < dist_to[GraphNode])
			{
				QGraphNode new_GraphNode = QGraphNode(GraphNode, dist_to_GraphNode + Distance2D(GraphNode->pos, end->pos));
				heap.push(new_GraphNode);
				dist_to[GraphNode] = dist_to_GraphNode;
				came_from[GraphNode] = current.node;
			}
		}

		visited.push_back(current.node);
	}
	// Error
	return {};
}

}