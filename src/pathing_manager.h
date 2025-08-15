#pragma once
#include <string>
#include <vector>
#include <map>

#include "sc2api/sc2_common.h"

#include "utility.h"

namespace sc2 {

class Mediator;

enum class NodeControl
{
	neutral,
	innactive,
	friendly_control,
	enemy_control
};

struct Node
{
	int uid = -1;
	Point2D pos = Point2D(0, 0);
	NodeControl control = NodeControl::neutral;
	Node* left_node = nullptr;
	Node* right_node = nullptr;
	std::vector<Node*> connections;
	Node(int id, Point2D point)
	{
		uid = id;
		pos = point;
	}
	std::string ConnectionsString()
	{
		std::string str = "";
		str += std::to_string(pos.x);
		str += ", ";
		str += std::to_string(pos.y);
		str += '\n';
		for (const auto& connection : connections)
		{
			str += std::to_string(connection->pos.x);
			str += ", ";
			str += std::to_string(connection->pos.y);
			str += "\n";
		}
		str += "end\n";
		if (left_node)
			str += left_node->ConnectionsString();
		else
			str += "null\n";
		if (right_node)
			str += right_node->ConnectionsString();
		else
			str += "null\n";
		return str;
	}
	std::string ToString()
	{
		std::string str = "";
		str += std::to_string(pos.x);
		str += ", ";
		str += std::to_string(pos.y);
		str += "\n";
		switch (control)
		{
		case NodeControl::neutral:
			str += "neutral\n";
			break;
		case NodeControl::innactive:
			str += "innactive\n";
			break;
		case NodeControl::friendly_control:
			str += "friendly_control\n";
			break;
		case NodeControl::enemy_control:
			str += "enemy_control\n";
			break;
		default:
			str += "default\n";
			break;
		}
		if (left_node)
			str += left_node->ToString();
		else
			str += "null";
		str += "\n";
		if (right_node)
			str += right_node->ToString();
		else
			str += "null";
		return str;
	}
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

typedef std::function<bool(const Node& node)> NodeFilter;

struct NodeControlFilter
{
	explicit NodeControlFilter(NodeControl control_);

	bool operator()(const Node& node_) const;

private:
	NodeControl m_control;
};

struct NodePointFilter
{
	explicit NodePointFilter(Point2D point_);

	bool operator()(const Node& node_) const;

private:
	Point2D m_point;
};

struct NodeCompare
{
	bool operator()(const QNode& node1, const QNode& node2)
	{
		return node1.dist > node2.dist;
	}
};

class KDTree
{
private:
	Node* root_node = nullptr;
	int next_node_id = 0;
	std::vector<Node*> all_nodes;
	int GetNextNodeId();
	Node* CreateNode(Point2D);
	Node* CreateKDTree(std::vector<Point2D>, int, std::map<OrderedPoint2D, Node*>&);
	void AddConnections(std::map<OrderedPoint2D, Node*>&, std::map<OrderedPoint2D, std::vector<Point2D>>&);
	void AddNoise(Node*);
	Node* FindClosestNode(Point2D, Node*, int) const;
	Node* CreateNodeFromFile(std::ifstream*, std::map<OrderedPoint2D, Node*>&);
	void AddConnectionsFromFile(Node*, std::ifstream*, std::map<OrderedPoint2D, Node*>&);
	void DisplayNode(Node*, Mediator*) const;

public:
	KDTree() {};
	KDTree(std::string);
	KDTree(std::vector<Point2D>, std::map<OrderedPoint2D, std::vector<Point2D>>&);
	Point2D FindClosestPoint(Point2D) const;
	std::pair<Point2D, NodeControl> FindClosestPointWithControl(Point2D) const;
	Node* FindClosestNode(Point2D) const;
	void SaveToFile(std::string);
	void DisplayTree(Mediator*) const;
	int GetNumNodes() const;
	Node* GetNodeFromUID(int) const;
};

class PathingManager
{
	friend class TheBigBot;
private:
	const int max_connection_depth = 25;
	Mediator* mediator;
	KDTree map_skeleton;
	Point2D central_base_pos;

	std::map<Node*, std::vector<Node*>> BFS(Node*); // do i need this? is it faster?
	std::vector<Point2D> FindPath(Point2D, NodeFilter) const;
	Node* FindClosestSkeletonNode(Point2D) const;
	std::vector<Point2D> ReconstructPathIndexed(std::vector<int>, Node*) const;
	void ChangeAreaControl(Point2D, Node*, float, NodeControl);
	void ChangeAreaControl(Point2D, Node*, float, float, NodeControl);
	bool ConnectAreaControl(Node*, std::vector<Node*>, int, NodeControl);
	void UpdateCentralBasePos();

public:
	PathingManager(Mediator* mediator)
	{
		this->mediator = mediator;
	}

	void DisplayMapSkeleton() const;
	void LoadMapData();
	Point2D FindClosestSkeletonPoint(Point2D) const;
	std::pair<Point2D, NodeControl> FindClosestSkeletonPointWithControl(Point2D) const;
	std::vector<Point2D> FindPath(Point2D, Point2D) const;
	std::vector<Point2D> FindPathToFriendlyControlledArea(Point2D) const;
	std::vector<Point2D> FindPathToEnemyControlledArea(Point2D) const;
	void ChangeAreaControl(Point2D, float, NodeControl);
	void ChangeAreaControl(Point2D, float, float, NodeControl);
	Point2D GetCentralBasePos() const;
};

}
