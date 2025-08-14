
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

int KDTree::GetNextNodeId()
{
	return next_node_id++;
}

Node* KDTree::CreateNode(Point2D pos)
{
	Node* node = new Node(GetNextNodeId(), pos);
	all_nodes.push_back(node);
	return node;
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
		Node* new_node = CreateNode(points[mid]);
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
		Node* new_node = CreateNode(points[mid]);
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

// add noise to avoid point2s falling on vertical/horizontal lines
void KDTree::AddNoise(Node* node)
{
	if (node == nullptr)
		return;
	
	float x_r = (float)(rand() - (RAND_MAX / 2)) / (RAND_MAX * 10);
	float y_r = (float)(rand() - (RAND_MAX / 2)) / (RAND_MAX * 10);
	node->pos += Point2D(x_r, y_r);

	AddNoise(node->left_node);
	AddNoise(node->right_node);
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


	Node* node = CreateNode(Point2D(x, y));

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
	AddNoise(root_node);

	tree_file.close();
}

KDTree::KDTree(std::vector<Point2D> points, std::map<OrderedPoint2D, std::vector<Point2D>>& all_connections)
{
	std::map<OrderedPoint2D, Node*> node_lookup;
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

std::pair<Point2D, NodeControl> KDTree::FindClosestPointWithControl(Point2D point) const
{
	Node* node = FindClosestNode(point, root_node, 0);
	if (node == nullptr)
		return std::make_pair(Point2D(0, 0), NodeControl::innactive);
	return std::make_pair(node->pos, node->control);
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

int KDTree::GetNumNodes() const
{
	return next_node_id;
}

Node* KDTree::GetNodeFromUID(int uid) const
{
	if (uid < 0 || uid >= all_nodes.size())
		return nullptr;
	return all_nodes[uid];
}

std::vector<Point2D> PathingManager::FindPath(Point2D starting_point, NodeFilter filter) const
{
	Node* start = FindClosestSkeletonNode(starting_point);

	std::priority_queue<QNode, std::vector<QNode>, NodeCompare> heap;
	int num_nodes = map_skeleton.GetNumNodes();
	std::vector<int> came_from(num_nodes, -1);
	std::vector<float> dist_to(num_nodes, INFINITY);

	heap.emplace(start, 0);
	dist_to[start->uid] = 0;

	while (!heap.empty())
	{
		QNode current = heap.top();
		float current_dist = current.dist;
		Node* current_node = current.node;
		heap.pop();
		if (current_dist > dist_to[current_node->uid])
			continue;
		if (filter(*current_node))
			return ReconstructPathIndexed(came_from, current_node);

		for (const auto& node : current_node->connections)
		{
			float dist_to_node = dist_to[current_node->uid] + DistanceSquared2D(current_node->pos, node->pos);

			if (dist_to_node < dist_to[node->uid])
			{
				heap.emplace(node, dist_to_node);
				dist_to[node->uid] = dist_to_node;
				came_from[node->uid] = current_node->uid;
			}
		}
	}
	// Error no path found
	return {};
}

Node* PathingManager::FindClosestSkeletonNode(Point2D point) const
{
	return map_skeleton.FindClosestNode(point);
}

std::vector<Point2D> PathingManager::ReconstructPathIndexed(std::vector<int> came_from, Node* end) const
{
	std::vector<Point2D> path;
	int current = end->uid;
	while (current != -1)
	{
		path.push_back(map_skeleton.GetNodeFromUID(current)->pos);
		current = came_from[current];
	}

	std::reverse(path.begin(), path.end());
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

void PathingManager::UpdateCentralBasePos()
{
	Node* starting_node = FindClosestSkeletonNode(mediator->GetStartLocation());
	std::vector<Point2D> end_points = { starting_node->pos };
	std::vector<Point2D> friendly_controled_nodes;
	std::vector<Node*> nodes_to_test = { starting_node };
	while (nodes_to_test.size() > 0)
	{
		Node* curr_node = nodes_to_test.back();
		nodes_to_test.pop_back();
		if (std::find(friendly_controled_nodes.begin(), friendly_controled_nodes.end(), curr_node->pos) != friendly_controled_nodes.end())
			continue;
		if (curr_node->control != NodeControl::friendly_control)
			continue;

		friendly_controled_nodes.push_back(curr_node->pos);

		for (const auto connection : curr_node->connections)
		{
			if (connection->control == NodeControl::neutral && std::find(end_points.begin(), end_points.end(), curr_node->pos) == end_points.end())
			{
				end_points.push_back(curr_node->pos); // intentionally double to weight end points higher that the starting point
				end_points.push_back(curr_node->pos);
			}
			else if (connection->control == NodeControl::friendly_control)
				nodes_to_test.push_back(connection);
		}
	}

	Point2D center_ends = Utility::Center(end_points);
	Node* center_point_ends_node = FindClosestSkeletonNode(center_ends);
	Point2D center_point_ends = center_point_ends_node->pos;
	if (center_point_ends_node->control != NodeControl::friendly_control)
	{
		std::vector<Point2D> path = FindPathToFriendlyControlledArea(center_point_ends_node->pos);
		if (path.size() > 0)
			center_point_ends = path.back();
	}

	central_base_pos = center_point_ends;
}

void PathingManager::DisplayMapSkeleton() const
{
	map_skeleton.DisplayTree(mediator);
	mediator->DebugSphere(mediator->ToPoint3D(central_base_pos), 3, Color(255, 255, 0));
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

std::pair<Point2D, NodeControl> PathingManager::FindClosestSkeletonPointWithControl(Point2D point) const
{
	return map_skeleton.FindClosestPointWithControl(point);
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
	if (control == NodeControl::friendly_control)
		UpdateCentralBasePos();
}

void PathingManager::ChangeAreaControl(Point2D point, float radius, float height, NodeControl control)
{
	Node* starting_node = FindClosestSkeletonNode(point);
	ChangeAreaControl(point, starting_node, radius, height, control);
	if (control == NodeControl::friendly_control)
		UpdateCentralBasePos();
}

Point2D PathingManager::GetCentralBasePos() const
{
	return central_base_pos;
}

}