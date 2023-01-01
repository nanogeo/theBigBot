#pragma once
#include "nav_mesh_pathfinding.h"

#include <iostream>
#include <string>
#include <queue>




namespace sc2 {

std::vector<Triangle*> NavMesh::ReconstructPath(std::map<Triangle*, Triangle*> came_from, Triangle* end)
{
	std::vector<Triangle*> path_r;
	Triangle* current = end;
	while (current != NULL)
	{
		path_r.push_back(current);
		current = came_from[current];
	}
	std::vector<Triangle*> path;
	for (int i = path_r.size() - 1; i >= 0; i--)
	{
		path.push_back(path_r[i]);
	}
	return path;
}

Triangle* NavMesh::FindClosestTriangle(Point2D pos)
{
	for (const auto &triangle : triangles)
	{
		if (PointInTriangle(triangle, pos))
			return triangle;
	}
	Triangle* closest = NULL;
	float dist = INFINITY;
	for (const auto &triangle : triangles)
	{
		if (Distance2D(pos, triangle->center) < dist)
		{
			closest = triangle;
			dist = Distance2D(pos, triangle->center);
		}
	}
	if (closest == NULL)
		std::cout << "Error no triangle found in FindClosestTriangle\n";
	return closest;
}

std::vector<Point2D> NavMesh::FindPath(Point2D start, Point2D end)
{
	Triangle* start_tri = FindClosestTriangle(start);
	Triangle* end_tri = FindClosestTriangle(end);
	if (start_tri == end_tri)
	{
		std::vector<Point2D> path;
		path.push_back(end);
		return path;
	}

	std::vector<Triangle*> tri_path = FindTrianglePath(start_tri, end_tri);

	std::vector<Portal> portals;
	for (int i = 0; i < tri_path.size() - 1; i++)
	{
		portals.push_back(FindPortal(tri_path[i], tri_path[i + 1]));
	}



	std::vector<Point2D> path = FunnelPoints(portals, start, end);

	return path;
}

std::vector<Triangle*> NavMesh::FindTrianglePath(Triangle* start, Triangle* end)
{
	auto compare = [](QTriangle a, QTriangle b) { return a.dist > b.dist; };
	std::priority_queue<QTriangle, std::vector<QTriangle>, decltype(compare)> heap(compare);
	std::map<Triangle*, Triangle*> came_from;
	std::map<Triangle*, float> dist_to;
	std::vector<Triangle*> visited;

	heap.push(QTriangle(start, Distance2D(start->center, end->center)));
	dist_to[start] = 0;
	came_from[start] = NULL;

	while (!heap.empty())
	{
		QTriangle current = heap.top();
		heap.pop();
		if (std::find(visited.begin(), visited.end(), current.triangle) != visited.end())
			continue;
		if (current.triangle == end)
			return ReconstructPath(came_from, end);


		for (const auto &node : current.triangle->connections)
		{
			float dist_to_node = dist_to[current.triangle] + Distance2D(current.triangle->center, node->center);
			if (dist_to.count(node) == 0 || dist_to_node < dist_to[node])
			{
				QTriangle new_node = QTriangle(node, dist_to_node + Distance2D(node->center, end->center));
				heap.push(new_node);
				dist_to[node] = dist_to_node;
				came_from[node] = current.triangle;
			}
		}

		visited.push_back(current.triangle);
	}
}

float NavMesh::TriangleSignPoint(Point2D point, Point2D start, Point2D end)
{
	return (point.x - end.x) * (start.y - end.y) - (start.x - end.x) * (point.y - end.y);
}

bool NavMesh::PointInTriangle(Triangle* triangle, Point2D point)
{
	float side1 = TriangleSignPoint(point, triangle->verticies[0], triangle->verticies[1]);
	float side2 = TriangleSignPoint(point, triangle->verticies[1], triangle->verticies[2]);
	float side3 = TriangleSignPoint(point, triangle->verticies[2], triangle->verticies[0]);

	bool has_neg = (side1 < 0) || (side2 < 0) || (side3 < 0);
	bool has_pos = (side1 > 0) || (side2 > 0) || (side3 > 0);

	return !(has_neg && has_pos);
}

Portal NavMesh::FindPortal(Triangle* start, Triangle* end)
{
	Point2D point1 = Point2D(0, 0);
	Point2D point2 = Point2D(0, 0);
	for (const auto &p1 : start->verticies)
	{
		for (const auto &p2 : end->verticies)
		{
			if (p1 == p2)
			{
				if (point1 == Point2D(0, 0))
					point1 = p1;
				else
					point2 = p1;
			}
		}
	}

	if (point1 == Point2D(0, 0) || point2 == Point2D(0, 0))
		std::cout << "Error triangles aren't neighbors in FindPortal";
	
	Portal portal;
	float orientation = ((point1.x - start->center.x) * (point2.y - start->center.y)) - ((point1.y - start->center.y) * (point2.x - start->center.x));
	if (orientation < 0)
	{
		portal.left = point1;
		portal.right = point2;
		return portal;
	}
	else if (orientation > 0)
	{
		portal.left = point2;
		portal.right = point1;
		return portal;
	}
	else
	{
		std::cout << "Error invalid orientation in FindPortal";
		return portal;
	}

}

std::vector<Point2D> NavMesh::FunnelPoints(std::vector<Portal> portals, Point2D start, Point2D end)
{
	std::vector<Point2D> path;
	path.push_back(start);
	Point2D current = start;
	int indexR = 0;
	int indexL = 0;
	int indexNew = 0;

	while (current != end)
	{
		Point2D left = portals[indexL].left;
		Point2D right = portals[indexR].right;
		indexNew++;

		if (indexNew >= portals.size())
		{
			current = end;
			break;
		}

		// try left
		Point2D new_left = portals[indexNew].left;
		if (left != new_left)
		{
			if (Cross(current, left, new_left) > 0)
			{
				// ignore point
			}
			else if (Cross(current, right, new_left) < 0)
			{
				current = right;
				path.push_back(current);
				int next_portal_index = FindNextRightPoint(portals, indexR);
				if (next_portal_index == -1)
				{
					current = end;
				}
				else
				{
					indexNew = next_portal_index;
					indexL = next_portal_index;
					indexR = next_portal_index;
				}
				continue;
			}
			else
			{
				indexL = indexNew;
			}
		}

		// try right
		Point2D new_right = portals[indexNew].right;
		if (right != new_right)
		{
			if (Cross(current, right, new_right) < 0)
			{
				// ignore point
			}
			else if (Cross(current, left, new_right) > 0)
			{
				current = left;
				path.push_back(current);
				int next_portal_index = FindNextLeftPoint(portals, indexL);
				if (next_portal_index == -1)
				{
					current = end;
				}
				else
				{
					indexNew = next_portal_index;
					indexL = next_portal_index;
					indexR = next_portal_index;
				}
				continue;
			}
			else
			{
				indexR = indexNew;
			}
		}
	}
	path.push_back(current);
	return path;
}

float NavMesh::Cross(Point2D origin, Point2D point1, Point2D point2)
{
	return ((point1.x - origin.x) * (point2.y - origin.y)) - ((point1.y - origin.y) * (point2.x - origin.x));
}

int NavMesh::FindNextRightPoint(std::vector<Portal> portals, int indexR)
{
	Point2D right = portals[indexR].right;
	int i = 0;
	Point2D new_right = portals[indexR + i].right;
	while (new_right == right)
	{
		i++;
		if (indexR + i >= portals.size())
			return -1;
		new_right = portals[indexR + i].right;
	}
	return indexR + i;
}

int NavMesh::FindNextLeftPoint(std::vector<Portal> portals, int indexL)
{
	Point2D left = portals[indexL].left;
	int i = 0;
	Point2D new_left = portals[indexL + i].left;
	while (new_left == left)
	{
		i++;
		if (indexL + i >= portals.size())
			return -1;
		new_left = portals[indexL + i].left;
	}
	return indexL + i;
}

}