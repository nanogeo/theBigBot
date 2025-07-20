#pragma once
#include <string>
#include <vector>
#include <map>

#include "sc2api/sc2_common.h"

namespace sc2 {

struct Node
{
    Point2D pos;
    std::vector<Node*> connections;
    bool is_exit = false;
    bool is_base = false;

};

struct QNode
{
    float dist;
    Node* node;
    QNode(Node* node, float dist)
    {
        this->node = node;
        this->dist = dist;
    }
};

struct NodeCompare
{
    bool operator()(const QNode& node1, const QNode& node2)
    {
        return node1.dist < node2.dist;
    }
};

class Graph
{
public:
    std::vector<Node*> nodes;

    Graph(std::vector<Point2D> points, std::vector<std::vector<int>> links)
    {
        for (int i = 0; i < points.size(); i++)
        {
            Node* node = new Node();
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

    std::vector<Node*> ReconstructPath(std::map<Node*, Node*>, Node*) const;
    Node* FindClosestNode(Point2D) const;
    std::vector<Node*> FindPath(Node*, Node*) const;
    Node* FindExit(Node*);
    std::vector<Node*> FindExitPath(Node*);
};

}
