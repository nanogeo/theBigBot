#pragma once
#include <string>
#include <vector>
#include <map>

#include "sc2api/sc2_common.h"

namespace sc2 {

struct GraphNode
{
    Point2D pos;
    std::vector<GraphNode*> connections;
};

struct QGraphNode
{
    float dist;
	GraphNode* node;
    QGraphNode(GraphNode* node, float dist)
    {
        this->node = node;
        this->dist = dist;
    }
};

struct GraphNodeCompare
{
    bool operator()(const QGraphNode& node1, const QGraphNode& node2)
    {
        return node1.dist < node2.dist;
    }
};

class Graph
{
public:
    std::vector<GraphNode*> nodes;
    std::map<Point2D, GraphNode*> node_lookup;

    Graph(std::vector<Point2D> points, std::vector<std::vector<int>> links)
    {
        for (int i = 0; i < points.size(); i++)
        {
			GraphNode* node = new GraphNode();
            node->pos = points[i];
            nodes.push_back(node);
        }
        for (int i = 0; i < nodes.size(); i++)
        {
            for (int j = 0; j < links[i].size(); j++)
            {
                nodes[i]->connections.push_back(nodes[links[i][j]]);
            }
        }
    }

    std::vector<GraphNode*> ReconstructPath(std::map<GraphNode*, GraphNode*>, GraphNode*) const;
	GraphNode* FindClosestNode(Point2D) const;
    std::vector<GraphNode*> FindPath(GraphNode*, GraphNode*) const;
	GraphNode* FindExit(GraphNode*);
    std::vector<GraphNode*> FindExitPath(GraphNode*);
};

}
