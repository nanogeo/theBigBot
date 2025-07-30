
#include "mediator.h"
#include "pathing_manager.h"

#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include <filesystem>



namespace sc2 {


NodeControlFilter::NodeControlFilter(NodeControl control_) : m_control(control_)
{
}

bool NodeControlFilter::operator()(const Node& node_) const {
	return node_.control == m_control;
}

NodePointFilter::NodePointFilter(Point2D point_) : m_point(point_)
{
}

bool NodePointFilter::operator()(const Node& node_) const {
	return node_.pos == m_point;
}


Node* KDTree::CreateKDTree(std::vector<Point2D> points, int depth, std::map<OrderedPoint2D, Node*>& node_lookup)
{
	if (depth % 2 == 0)
	{
		// x
		std::sort(points.begin(), points.end(), [](const Point2D& a, const Point2D& b) -> bool
		{
			return a.x < b.x;
		});
		int mid = (int)points.size() / 2;
		std::vector<Point2D> left_points;
		std::vector<Point2D> right_points;
		for (int i = 0; i < points.size(); i++)
		{
			if (i < mid)
				left_points.push_back(points[i]);
			else if (i > mid)
				right_points.push_back(points[i]);
		}
		Node* new_node = new Node();
		new_node->pos = points[mid];
		node_lookup[new_node->pos] = new_node;
		if (left_points.size() > 0)
			new_node->left_node = CreateKDTree(left_points, depth + 1, node_lookup);
		if (right_points.size() > 0)
			new_node->right_node = CreateKDTree(right_points, depth + 1, node_lookup);
		return new_node;
	}
	else
	{
		// y
		std::sort(points.begin(), points.end(), [](const Point2D& a, const Point2D& b) -> bool
		{
			return a.y < b.y;
		});
		int mid = (int)points.size() / 2;
		std::vector<Point2D> left_points;
		std::vector<Point2D> right_points;
		for (int i = 0; i < points.size(); i++)
		{
			if (i < mid)
				left_points.push_back(points[i]);
			else if (i > mid)
				right_points.push_back(points[i]);
		}
		Node* new_node = new Node();
		new_node->pos = points[mid];
		node_lookup[new_node->pos] = new_node;
		if (left_points.size() > 0)
			new_node->left_node = CreateKDTree(left_points, depth + 1, node_lookup);
		if (right_points.size() > 0)
			new_node->right_node = CreateKDTree(right_points, depth + 1, node_lookup);
		return new_node;

	}
}

void KDTree::AddConnections(std::map<OrderedPoint2D, Node*>& node_lookup, std::map<OrderedPoint2D, std::vector<Point2D>>& all_connections)
{
	for (const auto& node : all_connections)
	{
		if (node_lookup.find(node.first) == node_lookup.end())
			throw;
		Node* current_node = node_lookup.at(node.first);
		for (const auto& connection : node.second)
		{
			if (node_lookup.find(connection) == node_lookup.end())
				throw;
			Node* connecting_node = node_lookup.at(connection);
			current_node->connections.push_back(connecting_node);
		}
	}
}

Node* KDTree::FindClosestNode(Point2D point, Node* current_node, int depth) const
{
	if (current_node == nullptr)
		return nullptr;

	Node* next_node = nullptr;
	Node* alt_node = nullptr;
	if ((depth % 2 == 0 && point.x < current_node->pos.x) || (depth % 2 == 1 && point.y < current_node->pos.y))
	{
		// left
		next_node = current_node->left_node;
		alt_node = current_node->right_node;
	}
	else
	{
		// right
		next_node = current_node->right_node;
		alt_node = current_node->left_node;
	}

	Node* temp_node = FindClosestNode(point, next_node, depth + 1);
	Node* best_node = current_node;
	if (temp_node != nullptr)
		best_node = Distance2D(temp_node->pos, point) < Distance2D(current_node->pos, point) ? temp_node : current_node;

	float best_dist = Distance2D(best_node->pos, point);
	float alt_dist = depth % 2 == 0 ? std::abs(point.x - current_node->pos.x) : std::abs(point.y - current_node->pos.y);
	if (alt_dist < best_dist)
	{
		temp_node = FindClosestNode(point, alt_node, depth + 1);
		if (temp_node != nullptr && Distance2D(temp_node->pos, point) < best_dist)
			best_node = temp_node;
	}

	return best_node;
}

Node* KDTree::CreateNodeFromFile(std::ifstream* tree_file, std::map<OrderedPoint2D, Node*>& node_lookup)
{
	std::string pos = "";
	std::getline(*tree_file, pos);
#ifdef BUILD_FOR_LADDER
	pos.erase(pos.length() - 1);
#endif // BUILD_FOR_LADDER

	if (std::strcmp(pos.c_str(), "null") == 0)
		return nullptr;

	float x = std::stof(pos.substr(0, pos.find(',')));
	float y = std::stof(pos.substr(pos.find(',') + 1, pos.length() - 1));


	Node* node = new Node();
	node->pos = Point2D(x, y);

	std::string control = "";
	std::getline(*tree_file, control);
#ifdef BUILD_FOR_LADDER
	control.erase(control.length() - 1);
#endif // BUILD_FOR_LADDER
	if (std::strcmp(control.c_str(), "innactive") == 0)
		node->control = NodeControl::innactive;
	else if (std::strcmp(control.c_str(), "neutral") == 0)
		node->control = NodeControl::neutral;

	node_lookup[node->pos] = node;
	node->left_node = CreateNodeFromFile(tree_file, node_lookup);
	node->right_node = CreateNodeFromFile(tree_file, node_lookup);
	return node;
}

void KDTree::AddConnectionsFromFile(Node* curr_node, std::ifstream* tree_file, std::map<OrderedPoint2D, Node*>& node_lookup)
{
	std::string point = "";
	std::getline(*tree_file, point);
#ifdef BUILD_FOR_LADDER
	point.erase(point.length() - 1);
#endif // BUILD_FOR_LADDER
	if (curr_node == nullptr && std::strcmp(point.c_str(), "null") == 0)
		return;
	if (curr_node == nullptr || std::strcmp(point.c_str(), "null") == 0)
		throw;

	std::getline(*tree_file, point);
#ifdef BUILD_FOR_LADDER
	point.erase(point.length() - 1);
#endif // BUILD_FOR_LADDER
	while (std::strcmp(point.c_str(), "end") != 0)
	{
		float x = std::stof(point.substr(0, point.find(',')));
		float y = std::stof(point.substr(point.find(',') + 1, point.length() - 1));
		Point2D pos = Point2D(x, y);
		if (node_lookup.find(pos) == node_lookup.end())
			throw;
		Node* connection = node_lookup.at(pos);
		curr_node->connections.push_back(connection);
		std::getline(*tree_file, point);
#ifdef BUILD_FOR_LADDER
		point.erase(point.length() - 1);
#endif // BUILD_FOR_LADDER
	}
	AddConnectionsFromFile(curr_node->left_node, tree_file, node_lookup);
	AddConnectionsFromFile(curr_node->right_node, tree_file, node_lookup);
}

void KDTree::DisplayNode(Node* node, Mediator* mediator) const
{
	if (node == nullptr)
		return;

	switch (node->control)
	{
	case NodeControl::neutral:
		mediator->DebugSphere(mediator->ToPoint3D(node->pos), .5, Color(255, 255, 255));
		break;
	case NodeControl::innactive:
		mediator->DebugSphere(mediator->ToPoint3D(node->pos), .5, Color(0, 0, 0));
		break;
	case NodeControl::friendly_control:
		mediator->DebugSphere(mediator->ToPoint3D(node->pos), .5, Color(0, 255, 0));
		break;
	case NodeControl::enemy_control:
		mediator->DebugSphere(mediator->ToPoint3D(node->pos), .5, Color(255, 0, 0));
		break;
	}
	DisplayNode(node->left_node, mediator);
	DisplayNode(node->right_node, mediator);
}

// pull from file
KDTree::KDTree(std::string file_name)
{
	std::ifstream tree_file;
	tree_file.open(file_name, std::ios_base::in);
	std::map<OrderedPoint2D, Node*> node_lookup;

	root_node = CreateNodeFromFile(&tree_file, node_lookup);
	AddConnectionsFromFile(root_node, &tree_file, node_lookup);

	tree_file.close();
}

KDTree::KDTree(std::vector<Point2D> points, std::map<OrderedPoint2D, std::vector<Point2D>>& all_connections)
{
	std::map<OrderedPoint2D, Node*> node_lookup;
	// add noise to avoid point2s falling on vertical/horizontal lines
	/*for (auto& point : points)
	{
		float x_r = (float)(rand() % 100000 - 50000) / 1000000;
		float y_r = (float)(rand() % 100000 - 50000) / 1000000;
		point += Point2D(x_r, y_r);
	}*/
	root_node = CreateKDTree(points, 0, node_lookup);
	AddConnections(node_lookup, all_connections);

}

Point2D KDTree::FindClosestPoint(Point2D point) const
{
	Node* node = FindClosestNode(point, root_node, 0);
	if (node == nullptr)
		return Point2D(0, 0);
	return node->pos;
}

Node* KDTree::FindClosestNode(Point2D point) const
{
	return FindClosestNode(point, root_node, 0);
}

void KDTree::SaveToFile(std::string file_name)
{
	std::ofstream tree_file;
	tree_file.open(file_name, std::ios_base::out);

	tree_file << root_node->ToString();
	tree_file << '\n';

	tree_file << root_node->ConnectionsString();
	tree_file << '\n';

	tree_file.close();

}

void KDTree::DisplayTree(Mediator* mediator) const
{
	DisplayNode(root_node, mediator);
}

std::vector<Point2D> PathingManager::FindPath(Point2D starting_point, NodeFilter filter) const
{
	Node* start = FindClosestSkeletonNode(starting_point);

	std::priority_queue<QNode, std::vector<QNode>, NodeCompare> heap;
	std::map<Node*, Node*> came_from;
	std::map<Node*, float> dist_to;
	std::vector<Node*> visited;


	heap.push(QNode(start, 0));
	dist_to[start] = 0;
	came_from[start] = nullptr;

	while (!heap.empty())
	{
		QNode current = heap.top();
		heap.pop();
		if (std::find(visited.begin(), visited.end(), current.node) != visited.end())
			continue;
		if (filter(*current.node))
			return ReconstructPath(came_from, current.node);


		for (const auto& node : current.node->connections)
		{
			float dist_to_node = dist_to[current.node] + Distance2D(current.node->pos, node->pos);
			if (dist_to.count(node) == 0 || dist_to_node < dist_to[node])
			{
				QNode new_node = QNode(node, dist_to_node);
				heap.push(new_node);
				dist_to[node] = dist_to_node;
				came_from[node] = current.node;
			}
		}

		visited.push_back(current.node);
	}
	// Error no path found
	return {};
}

Node* PathingManager::FindClosestSkeletonNode(Point2D point) const
{
	return map_skeleton.FindClosestNode(point);
}

std::vector<Point2D> PathingManager::ReconstructPath(std::map<Node*, Node*> came_from, Node* end) const
{
	std::vector<Point2D> path_r;
	Node* current = end;
	while (current != nullptr)
	{
		path_r.push_back(current->pos);
		current = came_from[current];
	}
	std::vector<Point2D> path;
	for (int i = (int)path_r.size() - 1; i >= 0; i--)
	{
		path.push_back(path_r[i]);
	}
	return path;
}

void PathingManager::ChangeAreaControl(Point2D start, Node* current_node, float radius, NodeControl control)
{
	if (current_node == nullptr || current_node->control == control)
		return;
	if (Distance2D(start, current_node->pos) > radius)
	{
		bool found_connection = false;
		for (auto& node : current_node->connections)
		{
			if (ConnectAreaControl(node, { current_node }, 1, control))
				found_connection = true;
		}
		if (found_connection)
			current_node->control = control;

		return;
	}
	current_node->control = control;
	for (auto& node : current_node->connections)
	{
		ChangeAreaControl(start, node, radius, control);
	}
}

void PathingManager::ChangeAreaControl(Point2D start, Node* current_node, float radius, float height, NodeControl control)
{
	if (current_node == nullptr || current_node->control == control)
		return;
	if (Distance2D(start, current_node->pos) > radius || mediator->ToPoint3D(current_node->pos).z != height)
	{
		bool found_connection = false;
		for (auto& node : current_node->connections)
		{
			if (ConnectAreaControl(node, { current_node }, 1, control))
				found_connection = true;
		}
		if (found_connection)
			current_node->control = control;

		return;
	}
	current_node->control = control;
	for (auto& node : current_node->connections)
	{
		ChangeAreaControl(start, node, radius, height, control);
	}
}

bool PathingManager::ConnectAreaControl(Node* current_node, std::vector<Node*> prev_nodes, int depth, NodeControl control)
{
	if (depth > max_connection_depth)
		return false;
	if (current_node->control == control)
		return true;

	bool found_connection = false;
	prev_nodes.push_back(current_node);
	for (auto& node : current_node->connections)
	{
		if (std::find(prev_nodes.begin(), prev_nodes.end(), node) != prev_nodes.end())
			continue;
		if (ConnectAreaControl(node, prev_nodes, depth + 1, control))
			found_connection = true;
	}
	if (found_connection)
	{
		current_node->control = control;
		return true;
	}
	return false;
}

void PathingManager::DisplayMapSkeleton() const
{
	map_skeleton.DisplayTree(mediator);
}

void PathingManager::LoadMapData()
{
	std::string map_skeleton_filename = mediator->GetMapName();
	map_skeleton_filename = map_skeleton_filename.substr(0, map_skeleton_filename.find(' ')) + "_skeleton.txt";

#ifdef BUILD_FOR_LADDER
	map_skeleton_filename = "data/data/" + map_skeleton_filename;
#else
	map_skeleton_filename = "../../data/" + map_skeleton_filename;
#endif

	if (std::filesystem::exists(std::filesystem::path(map_skeleton_filename)))
	{
		mediator->SendChat("Map data found", ChatChannel::All);
		map_skeleton = KDTree(map_skeleton_filename);
		return;
	}
	mediator->SendChat("No map data found", ChatChannel::All);
	std::cerr << map_skeleton_filename << std::endl;
	std::string path = ".";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) 
	{
		std::cerr << entry.path().filename() << std::endl;
	}


	// map skeleton file doesnt exist so create it
	ImageData raw_map = mediator->GetPathingGrid();
	std::vector<Point2D> map_points = mediator->GetLocations().map_points_temp;
	if (map_points.size() == 0)
		return; // this is only populated when initially creating the map skeleton

	std::vector<std::vector<bool>> map;
	std::map<OrderedPoint2D, std::vector<Point2D>> connections;
	for (int i = 0; i < raw_map.width; i++)
	{

		map.push_back({});
		for (int j = 0; j < raw_map.height; j++)
		{
			map[i].push_back(false);
		}
	}
	for (const auto& point : map_points)
	{
		map[point.x][point.y] = true;
	}
	for (const auto& point : map_points)
	{
		connections[point] = {};
		for (int i = -1; i < 2; i++)
		{
			if (point.x + i < 0 || point.x + i >= raw_map.width)
				continue;
			for (int j = -1; j < 2; j++)
			{
				if (point.y + j < 0 || point.y + j >= raw_map.height || (i == 0 && j == 0))
					continue;
				if (map[point.x + i][point.y + j])
					connections[point].push_back(point + Point2D(i, j));
			}
		}
	}
	map_skeleton = KDTree(map_points, connections);

	map_skeleton.SaveToFile(map_skeleton_filename);
}

Point2D PathingManager::FindClosestSkeletonPoint(Point2D point) const
{
	return map_skeleton.FindClosestPoint(point);
}

std::vector<Point2D> PathingManager::FindPath(Point2D starting_point, Point2D ending_point) const
{
	return FindPath(starting_point, NodePointFilter(ending_point));
}

std::vector<Point2D> PathingManager::FindPathToFriendlyControlledArea(Point2D starting_point) const
{
	return FindPath(starting_point, NodeControlFilter(NodeControl::friendly_control));
}

std::vector<Point2D> PathingManager::FindPathToEnemyControlledArea(Point2D starting_point) const
{
	return FindPath(starting_point, NodeControlFilter(NodeControl::enemy_control));
}

void PathingManager::ChangeAreaControl(Point2D point, float radius, NodeControl control)
{
	Node* starting_node = FindClosestSkeletonNode(point);
	ChangeAreaControl(point, starting_node, radius, control);
}

void PathingManager::ChangeAreaControl(Point2D point, float radius, float height, NodeControl control)
{
	Node* starting_node = FindClosestSkeletonNode(point);
	ChangeAreaControl(point, starting_node, radius, height, control);
}

}