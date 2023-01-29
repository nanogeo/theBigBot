#pragma once
#include "nav_mesh_pathfinding.h"

#include <iostream>
#include <string>
#include <queue>
#include <fstream>

#include "sc2api/sc2_map_info.h"




namespace sc2 {



#pragma region NavMesh Creation

void NavMesh::LoadOrBuildNavmesh(ImageData map, std::string map_name)
{
	if (BuildNavMeshFromFile(map_name + "_navmesh.txt"))
		std::cout << "Navmesh found!\n";
	else
	{
		std::cout << "no navmesh found, bulding navmesh\n";
		std::vector<std::vector<bool>> grid_map = SetUpMap(map);
		std::vector<Vec2D> isolines = FindIsolines(grid_map);

		std::vector<Polygon> polygons = MakePolygons(isolines);

		std::vector<Point2D> vertices = GetAllVerticiesWithIntersections(polygons, map);
		all_vertices = vertices;

		MakeSeparateTriangles(vertices, map);

		/*std::vector<Polygon*> all_triangles = MakeTriangles(vertices, map);
		triangles = ConvertToTriangles(all_triangles);
		triangles = MarkOutsideTriangles(triangles, grid_map);

		SaveNavMeshToFile(map_name + "_navmesh.txt");*/
	}
	nav_mesh_populated = true;
}

std::vector<std::vector<bool>> NavMesh::SetUpMap(ImageData raw_map)
{
	std::vector<std::vector<bool>> map;
	std::vector<bool> x;
	map.push_back(x);
	int position = 0;
	int byte = 0;
	int row = 0;
	for (int i = 0; i < raw_map.data.size() * 8; i++)
	{
		if (((raw_map.data[byte] >> (7 - position)) & 0x1))
		{
			map[row].push_back(true);
		}
		else
		{
			map[row].push_back(false);
		}
		position++;
		if (position == 8)
		{
			position = 0;
			byte++;
		}
		if (i != 0 && i % raw_map.width == 0)
		{
			row++;
			std::vector<bool> x;
			map.push_back(x);
			map[row].push_back(false);
		}
	}
	map[row].push_back(false);


	std::vector<std::vector<bool>> flipped_map;
	for (int i = 0; i < map[0].size(); i++)
	{
		std::vector<bool> x;
		flipped_map.push_back(x);
	}
	for (int i = 0; i < map.size(); i++)
	{
		for (int j = 0; j < map[i].size(); j++)
		{
			flipped_map[j].push_back(map[i][j]);
		}
	}

	//flipped_map = AddNeutralUnitsToMap(flipped_map);

	return flipped_map;
}

/*std::vector<std::vector<bool>> NavMesh::AddNeutralUnitsToMap(std::vector<std::vector<bool>> map)
{
	std::vector<Point2D> blockers;
	for (const auto *unit : Observation()->GetUnits())
	{
		switch (unit->unit_type.ToType())
		{
		case UNIT_TYPEID::NEUTRAL_MINERALFIELD750:
			blockers.push_back(unit->pos - Point2D(0, .5));
			blockers.push_back(unit->pos - Point2D(1, .5));
			break;
		case UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750:
			blockers.push_back(unit->pos - Point2D(0, .5));
			blockers.push_back(unit->pos - Point2D(1, .5));
			break;
		case UNIT_TYPEID::NEUTRAL_LABMINERALFIELD:
			blockers.push_back(unit->pos - Point2D(0, .5));
			blockers.push_back(unit->pos - Point2D(1, .5));
			break;
		case UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEDEBRISRAMPDIAGONALHUGEULBR:
			blockers.push_back(unit->pos + Point2D(0, 0));
			blockers.push_back(unit->pos + Point2D(0, -1));
			blockers.push_back(unit->pos + Point2D(-1, -1));
			blockers.push_back(unit->pos + Point2D(-1, 0));
			blockers.push_back(unit->pos + Point2D(1, -2));
			blockers.push_back(unit->pos + Point2D(1, -3));
			blockers.push_back(unit->pos + Point2D(2, -2));
			blockers.push_back(unit->pos + Point2D(2, -3));
			blockers.push_back(unit->pos + Point2D(3, -4));
			blockers.push_back(unit->pos + Point2D(1, 0));
			blockers.push_back(unit->pos + Point2D(1, -1));
			blockers.push_back(unit->pos + Point2D(2, -1));
			blockers.push_back(unit->pos + Point2D(3, -2));
			blockers.push_back(unit->pos + Point2D(3, -3));
			blockers.push_back(unit->pos + Point2D(4, -3));
			blockers.push_back(unit->pos + Point2D(-1, -2));
			blockers.push_back(unit->pos + Point2D(0, -2));
			blockers.push_back(unit->pos + Point2D(0, -3));
			blockers.push_back(unit->pos + Point2D(1, -4));
			blockers.push_back(unit->pos + Point2D(2, -4));
			blockers.push_back(unit->pos + Point2D(2, -5));
			blockers.push_back(unit->pos + Point2D(-2, 1));
			blockers.push_back(unit->pos + Point2D(-2, 2));
			blockers.push_back(unit->pos + Point2D(-3, 1));
			blockers.push_back(unit->pos + Point2D(-3, 2));
			blockers.push_back(unit->pos + Point2D(-4, 3));
			blockers.push_back(unit->pos + Point2D(0, 1));
			blockers.push_back(unit->pos + Point2D(-1, 1));
			blockers.push_back(unit->pos + Point2D(-1, 2));
			blockers.push_back(unit->pos + Point2D(-2, 3));
			blockers.push_back(unit->pos + Point2D(-3, 3));
			blockers.push_back(unit->pos + Point2D(-3, 4));
			blockers.push_back(unit->pos + Point2D(-2, -1));
			blockers.push_back(unit->pos + Point2D(-2, 0));
			blockers.push_back(unit->pos + Point2D(-3, 0));
			blockers.push_back(unit->pos + Point2D(-4, 1));
			blockers.push_back(unit->pos + Point2D(-4, 2));
			blockers.push_back(unit->pos + Point2D(-5, 2));
			break;
		case UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER:
			blockers.push_back(unit->pos - Point2D(1, -1));
			blockers.push_back(unit->pos - Point2D(0, -1));
			blockers.push_back(unit->pos - Point2D(-1, -1));
			blockers.push_back(unit->pos - Point2D(1, 0));
			blockers.push_back(unit->pos - Point2D(0, 0));
			blockers.push_back(unit->pos - Point2D(-1, 0));
			blockers.push_back(unit->pos - Point2D(1, 1));
			blockers.push_back(unit->pos - Point2D(0, 1));
			blockers.push_back(unit->pos - Point2D(-1, 1));
			break;
		case UNIT_TYPEID::NEUTRAL_MINERALFIELD:
			blockers.push_back(unit->pos - Point2D(0, .5));
			blockers.push_back(unit->pos - Point2D(1, .5));
			break;
		case UNIT_TYPEID::NEUTRAL_VESPENEGEYSER:
			blockers.push_back(unit->pos - Point2D(1, -1));
			blockers.push_back(unit->pos - Point2D(0, -1));
			blockers.push_back(unit->pos - Point2D(-1, -1));
			blockers.push_back(unit->pos - Point2D(1, 0));
			blockers.push_back(unit->pos - Point2D(0, 0));
			blockers.push_back(unit->pos - Point2D(-1, 0));
			blockers.push_back(unit->pos - Point2D(1, 1));
			blockers.push_back(unit->pos - Point2D(0, 1));
			blockers.push_back(unit->pos - Point2D(-1, 1));
			break;
		case UNIT_TYPEID::NEUTRAL_UNBUILDABLEPLATESDESTRUCTIBLE:
			break;
		case UNIT_TYPEID::DESTRUCTIBLEDEBRIS4X4:
			blockers.push_back(unit->pos + Point2D(0, 0));
			blockers.push_back(unit->pos + Point2D(0, -1));
			blockers.push_back(unit->pos + Point2D(-1, 0));
			blockers.push_back(unit->pos + Point2D(-1, -1));
			blockers.push_back(unit->pos + Point2D(1, 0));
			blockers.push_back(unit->pos + Point2D(1, -1));
			blockers.push_back(unit->pos + Point2D(-2, 0));
			blockers.push_back(unit->pos + Point2D(-2, -1));
			blockers.push_back(unit->pos + Point2D(0, 1));
			blockers.push_back(unit->pos + Point2D(-1, 1));
			blockers.push_back(unit->pos + Point2D(0, -2));
			blockers.push_back(unit->pos + Point2D(-1, -2));
			break;
		case UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEDEBRIS6X6:
			blockers.push_back(unit->pos + Point2D(0, 0));
			blockers.push_back(unit->pos + Point2D(0, -1));
			blockers.push_back(unit->pos + Point2D(-1, 0));
			blockers.push_back(unit->pos + Point2D(-1, -1));
			blockers.push_back(unit->pos + Point2D(1, 0));
			blockers.push_back(unit->pos + Point2D(1, -1));
			blockers.push_back(unit->pos + Point2D(2, 0));
			blockers.push_back(unit->pos + Point2D(2, -1));
			blockers.push_back(unit->pos + Point2D(-2, 0));
			blockers.push_back(unit->pos + Point2D(-2, -1));
			blockers.push_back(unit->pos + Point2D(-3, 0));
			blockers.push_back(unit->pos + Point2D(-3, -1));
			blockers.push_back(unit->pos + Point2D(0, 1));
			blockers.push_back(unit->pos + Point2D(0, 2));
			blockers.push_back(unit->pos + Point2D(-1, 1));
			blockers.push_back(unit->pos + Point2D(-1, 2));
			blockers.push_back(unit->pos + Point2D(0, -2));
			blockers.push_back(unit->pos + Point2D(0, -3));
			blockers.push_back(unit->pos + Point2D(-1, -2));
			blockers.push_back(unit->pos + Point2D(-1, -3));
			blockers.push_back(unit->pos + Point2D(1, 1));
			blockers.push_back(unit->pos + Point2D(1, -2));
			blockers.push_back(unit->pos + Point2D(-2, -2));
			blockers.push_back(unit->pos + Point2D(-2, 1));
			break;
		case UNIT_TYPEID::DESTRUCTIBLEROCKEX16X6:
			blockers.push_back(unit->pos + Point2D(0, 0));
			blockers.push_back(unit->pos + Point2D(-1, 0));
			blockers.push_back(unit->pos + Point2D(0, -1));
			blockers.push_back(unit->pos + Point2D(-1, -1));
			blockers.push_back(unit->pos + Point2D(1, 0));
			blockers.push_back(unit->pos + Point2D(2, 0));
			blockers.push_back(unit->pos + Point2D(1, -1));
			blockers.push_back(unit->pos + Point2D(2, -1));
			blockers.push_back(unit->pos + Point2D(-2, 0));
			blockers.push_back(unit->pos + Point2D(-3, 0));
			blockers.push_back(unit->pos + Point2D(-2, -1));
			blockers.push_back(unit->pos + Point2D(-3, -1));
			blockers.push_back(unit->pos + Point2D(0, 1));
			blockers.push_back(unit->pos + Point2D(0, 2));
			blockers.push_back(unit->pos + Point2D(-1, 1));
			blockers.push_back(unit->pos + Point2D(-1, 2));
			blockers.push_back(unit->pos + Point2D(0, -2));
			blockers.push_back(unit->pos + Point2D(0, -3));
			blockers.push_back(unit->pos + Point2D(-1, -2));
			blockers.push_back(unit->pos + Point2D(-1, -3));
			blockers.push_back(unit->pos + Point2D(1, 1));
			blockers.push_back(unit->pos + Point2D(1, -2));
			blockers.push_back(unit->pos + Point2D(-2, 1));
			blockers.push_back(unit->pos + Point2D(-2, -2));
			break;
		default:
			std::cout << "Error unknown unit type in AddNeutralUnitsToMap\n";
			break;

		}
	}
	for (const auto &pos : blockers)
	{
		map[pos.x][pos.y] = false;
	}
	return map;
}*/

std::vector<Vec2D> NavMesh::FindIsolines(std::vector<std::vector<bool>> map)
{
	std::vector<Vec2D> all_isolines;
	for (int i = 0; i < map.size() - 1; i++)
	{
		for (int j = 0; j < map[i].size() - 1; j++)
		{
			Point2D t = Point2D(i + .5, j);
			Point2D b = Point2D(i + .5, j + 1);
			Point2D l = Point2D(i, j + .5);
			Point2D r = Point2D(i + 1, j + .5);

			std::vector<Vec2D> isolines = GetIsolineConfiguration(map[i][j], map[i + 1][j], map[i + 1][j + 1], map[i][j + 1], t, b, l, r);

			for (const auto &vec : isolines)
			{
				all_isolines.push_back(vec);
			}
		}
	}
	return all_isolines;
}

std::vector<Vec2D> NavMesh::GetIsolineConfiguration(bool w, bool x, bool y, bool z, Point2D t, Point2D b, Point2D l, Point2D r)
{
	std::vector<Vec2D> lines;
	int state = w * 8 + x * 4 + y * 2 + z * 1;
	t += Point2D(.5, .5);
	b += Point2D(.5, .5);
	l += Point2D(.5, .5);
	r += Point2D(.5, .5);
	switch (state)
	{
	case 0:
		break;
	case 1:
		lines.push_back(Vec2D(b, l));
		break;
	case 2:
		lines.push_back(Vec2D(b, r));
		break;
	case 3:
		lines.push_back(Vec2D(l, r));
		break;
	case 4:
		lines.push_back(Vec2D(t, r));
		break;
	case 5:
		lines.push_back(Vec2D(l, t));
		lines.push_back(Vec2D(b, r));
		break;
	case 6:
		lines.push_back(Vec2D(t, b));
		break;
	case 7:
		lines.push_back(Vec2D(l, t));
		break;
	case 8:
		lines.push_back(Vec2D(l, t));
		break;
	case 9:
		lines.push_back(Vec2D(t, b));
		break;
	case 10:
		lines.push_back(Vec2D(l, b));
		lines.push_back(Vec2D(t, r));
		break;
	case 11:
		lines.push_back(Vec2D(t, r));
		break;
	case 12:
		lines.push_back(Vec2D(l, r));
		break;
	case 13:
		lines.push_back(Vec2D(b, r));
		break;
	case 14:
		lines.push_back(Vec2D(l, b));
		break;
	case 15:
		break;
	default:
		std::cout << "Error invalid input in GetIsolineConfiguration";
		break;
	}
	return lines;
}

std::vector<Polygon> NavMesh::MakePolygons(std::vector<Vec2D> isolines)
{
	std::vector<Polygon> polygons;
	// loop through isolines
	while (isolines.size() > 0)
	{
		Point2D start = isolines[0].start;
		Point2D current_end = isolines[0].end;
		Polygon polygon;
		isolines.erase(isolines.begin());
		// start is saved
		polygon.points.push_back(start);
		// loop through isolines again and look for start == end
		for (int j = 0; j < isolines.size(); j++)
		{
			if (isolines[j].start == current_end)
			{
				polygon.points.push_back(current_end);
				current_end = isolines[j].end;
				isolines.erase(isolines.begin() + j);
				j = -1;
			}
			else if (isolines[j].end == current_end)
			{
				polygon.points.push_back(current_end);
				current_end = isolines[j].start;
				isolines.erase(isolines.begin() + j);
				j = -1;
			}
			if (start == current_end)
			{
				j = -1;
				break;
			}
		}
		Polygon newpoly = SimplifyPolygon(polygon);
		polygons.push_back(newpoly);
	}
	return polygons;
}

Polygon NavMesh::SimplifyPolygon(Polygon original)
{
	std::vector<Point2D> points = original.points;
	for (int i = 0; i < points.size();)
	{
		Point2D start = points[i];
		Point2D next;
		Point2D nextnext;
		if (i + 1 < points.size())
			next = points[i + 1];
		else
			next = points[i + 1 - points.size()];
		if (i + 2 < points.size())
			nextnext = points[i + 2];
		else
			nextnext = points[i + 2 - points.size()];

		Point2D vec1 = next - start;
		Point2D vec2 = nextnext - next;

		if (vec1.x * vec2.y - vec1.y * vec2.x == 0)
			points.erase(points.begin() + i + 1);
		else
			i++;
	}
	return Polygon(points);
}

std::vector<Point2D> NavMesh::GetAllVerticies(std::vector<Polygon> polygons)
{
	std::vector<Point2D> points;
	for (const auto &polygon : polygons)
	{
		for (const auto &point : polygon.points)
		{
			points.push_back(point);
		}
	}
	return points;
}

std::vector<Polygon*> NavMesh::MakeTriangles(std::vector<Point2D> verticies, ImageData raw_map)
{
	// make supra-triangle
	Polygon* supra_triangle = new Polygon();
	supra_triangle->points.push_back(Point2D(0, 0));
	supra_triangle->points.push_back(Point2D(0, raw_map.height * 2));
	supra_triangle->points.push_back(Point2D(raw_map.width * 2, 0));

	std::vector<Polygon*> triangles;
	triangles.push_back(supra_triangle);

	// loop through points
	for (const auto &point : verticies)
	{
		std::vector<Polygon*> bad_triangles;
		//   loop through triangles
		for (auto &triangle : triangles)
		{
			//      compute circumcircle
			Circle circumcircle = ComputeCircumcircle(*triangle);

			//      if point is within circle
			if (Distance2D(point, circumcircle.center) <= circumcircle.radius)
			{
				//          add triangle to bad triangles
				bad_triangles.push_back(triangle);
			}
		}
		std::vector<Vec2D> edges;
		//   for each triangle in bad triangles
		for (const auto &triangle : bad_triangles)
		{
			edges.push_back(Vec2D(triangle->points[0], triangle->points[1]));
			edges.push_back(Vec2D(triangle->points[1], triangle->points[2]));
			edges.push_back(Vec2D(triangle->points[2], triangle->points[0]));
			//      remove bad triangle from triangles
			triangles.erase(std::remove(triangles.begin(), triangles.end(), triangle), triangles.end());
		}
		// remove common edges
		for (int i = edges.size() - 1; i > 0; i--)
		{
			bool common = false;
			for (int j = i - 1; j >= 0; j--)
			{
				if (edges[i] == edges[j])
				{
					common = true;
					edges.erase(edges.begin() + j);
					i--;
				}
			}
			if (common)
				edges.erase(edges.begin() + i);
		}
		// make new triangles with edges
		for (const auto &edge : edges)
		{
			triangles.push_back(new Polygon({ point, edge.start, edge.end }));
		}
	}
	// delete any triangle with 2+ verticies the same as the supra triangle
	for (int i = triangles.size() - 1; i >= 0; i--)
	{
		if (DoesShareVertex(*triangles[i], *supra_triangle))
			triangles.erase(triangles.begin() + i);
	}
	return triangles;
}

Circle NavMesh::ComputeCircumcircle(Polygon triangle)
{
	if (triangle.points.size() != 3)
	{
		std::cout << "Error non-triangle passed into ComputerCircumcircle";
		return Circle();
	}
	Point2D a = triangle.points[0];
	Point2D b = triangle.points[1];
	Point2D c = triangle.points[2];
	float d = (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)) * 2;
	float x = ((pow(a.x, 2) + pow(a.y, 2)) * (b.y - c.y) + (pow(b.x, 2) + pow(b.y, 2)) * (c.y - a.y) + (pow(c.x, 2) + pow(c.y, 2)) * (a.y - b.y)) / d;
	float y = ((pow(a.x, 2) + pow(a.y, 2)) * (c.x - b.x) + (pow(b.x, 2) + pow(b.y, 2)) * (a.x - c.x) + (pow(c.x, 2) + pow(c.y, 2)) * (b.x - a.x)) / d;

	Circle circle;
	circle.center = Point2D(x, y);
	circle.radius = Distance2D(a, circle.center);
	return circle;
}

Circle NavMesh::ComputeCircumcircle(Triangle* triangle)
{
	if (triangle->verticies.size() != 3)
	{
		std::cout << "Error non-triangle passed into ComputerCircumcircle";
		return Circle();
	}
	Point2D a = triangle->verticies[0];
	Point2D b = triangle->verticies[1];
	Point2D c = triangle->verticies[2];
	float d = (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)) * 2;
	float x = ((pow(a.x, 2) + pow(a.y, 2)) * (b.y - c.y) + (pow(b.x, 2) + pow(b.y, 2)) * (c.y - a.y) + (pow(c.x, 2) + pow(c.y, 2)) * (a.y - b.y)) / d;
	float y = ((pow(a.x, 2) + pow(a.y, 2)) * (c.x - b.x) + (pow(b.x, 2) + pow(b.y, 2)) * (a.x - c.x) + (pow(c.x, 2) + pow(c.y, 2)) * (b.x - a.x)) / d;

	Circle circle;
	circle.center = Point2D(x, y);
	circle.radius = Distance2D(a, circle.center);
	return circle;
}

bool NavMesh::DoesShareVertex(Polygon poly1, Polygon poly2)
{
	int similar_verticies = 0;
	for (const auto &point1 : poly1.points)
	{
		for (const auto &point2 : poly2.points)
		{
			if (point1 == point2)
				similar_verticies++;
		}
	}
	return similar_verticies > 0;
}

bool NavMesh::DoesShareVertex(Triangle* tri1, Triangle* tri2)
{
	int similar_verticies = 0;
	for (const auto &point1 : tri1->verticies)
	{
		for (const auto &point2 : tri2->verticies)
		{
			if (point1 == point2)
				similar_verticies++;
		}
	}
	return similar_verticies > 0;
}

bool NavMesh::DoesShareSide(Triangle tri1, Triangle tri2)
{
	int similar_verticies = 0;
	for (const auto &point1 : tri1.verticies)
	{
		for (const auto &point2 : tri2.verticies)
		{
			if (point1 == point2)
				similar_verticies++;
		}
	}
	return similar_verticies >= 2;
}

std::vector<Triangle*> NavMesh::MarkOutsideTriangles(std::vector<Triangle*> triangles, std::vector<std::vector<bool>> grid_map)
{
	for (int i = triangles.size() - 1; i >= 0; i--)
	{
		Point2D center = Point2D((triangles[i]->verticies[0].x + triangles[i]->verticies[1].x + triangles[i]->verticies[2].x) / 3, (triangles[i]->verticies[0].y + triangles[i]->verticies[1].y + triangles[i]->verticies[2].y) / 3);
		if (!grid_map[floor(center.x)][floor(center.y)])
			triangles[i]->pathable = false;
	}
	return triangles;
}

std::vector<Triangle*> NavMesh::ConvertToTriangles(std::vector<Polygon*> polygons)
{
	std::vector<Triangle*> triangles;
	for (const auto *poly : polygons)
	{
		triangles.push_back(new Triangle(poly->points));
	}

	for (int i = 0; i < triangles.size() - 1; i++)
	{
		Triangle* tri1 = triangles[i];
		if (tri1->connections.size() == 3)
			continue;
		for (int j = i + 1; j < triangles.size(); j++)
		{
			Triangle* tri2 = triangles[j];
			if (DoesShareSide(*tri1, *tri2))
			{
				tri1->connections.push_back(tri2);
				tri2->connections.push_back(tri1);
				if (tri1->connections.size() == 3)
					break;
			}
		}
	}
	return triangles;
}




void NavMesh::AddNewBlocker(const Unit* unit)
{
	Polygon polygon = CreateBlockerPolygon(unit);
	std::vector<Triangle*> overlapping_triangles = FindOverlappingTriangles(polygon);
	std::vector<Point2D> intersection_points = FindIntersectionPoints(polygon, overlapping_triangles);
	std::vector<Point2D> removed_verticies = SaveVerticies(overlapping_triangles);
	//RemoveTriangles(overlapping_triangles);
	AddVerticies(intersection_points, polygon.points);
}

Polygon NavMesh::CreateBlockerPolygon(const Unit* unit)
{
	Polygon polygon;
	switch (unit->unit_type.ToType())
	{
	case UNIT_TYPEID::PROTOSS_PYLON:
		polygon.points.push_back(unit->pos + Point2D(3, 3));
		polygon.points.push_back(unit->pos + Point2D(3, -3));
		polygon.points.push_back(unit->pos + Point2D(-3, -3));
		polygon.points.push_back(unit->pos + Point2D(-3, 3));
		break;
	case UNIT_TYPEID::TERRAN_MISSILETURRET:
		polygon.points.push_back(unit->pos + Point2D(12, 3));
		polygon.points.push_back(unit->pos + Point2D(10, 7));
		polygon.points.push_back(unit->pos + Point2D(7, 10));
		polygon.points.push_back(unit->pos + Point2D(3, 12));
		polygon.points.push_back(unit->pos + Point2D(-4, 12));
		polygon.points.push_back(unit->pos + Point2D(-8, 10));
		polygon.points.push_back(unit->pos + Point2D(-11, 7));
		polygon.points.push_back(unit->pos + Point2D(-13, 3));
		polygon.points.push_back(unit->pos + Point2D(-13, -4));
		polygon.points.push_back(unit->pos + Point2D(-11, -8));
		polygon.points.push_back(unit->pos + Point2D(-8, -11));
		polygon.points.push_back(unit->pos + Point2D(-4, -13));
		polygon.points.push_back(unit->pos + Point2D(3, -13));
		polygon.points.push_back(unit->pos + Point2D(7, -11));
		polygon.points.push_back(unit->pos + Point2D(10, -8));
		polygon.points.push_back(unit->pos + Point2D(12, -4));
		break;
	default:
		break;
	}
		return polygon;
}

std::vector<Triangle*> NavMesh::FindOverlappingTriangles(Polygon polygon)
{
	std::vector<OverlapInfo> infos;
	for (int i = 0; i < polygon.points.size(); i++)
	{
		Point2D vec;
		if (i + 1 >= polygon.points.size())
			vec = polygon.points[0] - polygon.points[i];
		else
			vec = polygon.points[i + 1] - polygon.points[i];
		Point2D pvec = Point2D(-vec.y, vec.x);
		std::vector<float> projections;
		for (const auto &vertex : polygon.points)
		{
			projections.push_back(Dot2D(pvec, vertex));
		}
		OverlapInfo side_info;
		side_info.pvector = pvec;
		side_info.max = *std::max_element(std::begin(projections), std::end(projections));
		side_info.min = *std::min_element(std::begin(projections), std::end(projections));
		infos.push_back(side_info);
	}

	// TODO check using BFS instead of all triangles
	std::vector<Triangle*> possible_intersecting_triangles;
	for (const auto &triangle : triangles)
	{
		bool found_side = false;
		for (const auto &side : infos)
		{
			bool intersection = false;
			bool greater;
			if (Dot2D(triangle->verticies[0], side.pvector) > side.max)
				greater = true;
			else if (Dot2D(triangle->verticies[0], side.pvector) < side.min)
				greater = false;
			else
			{
				intersection = true;
				continue;
			}
			for (int i = 1; i < 3; i++)
			{
				if (Dot2D(triangle->verticies[i], side.pvector) <= side.max && greater)
				{
					intersection = true;
					break;
				}
				if (Dot2D(triangle->verticies[i], side.pvector) >= side.min && !greater)
				{
					intersection = true;
					break;
				}
			}
			if (!intersection)
			{
				found_side = true;
				break;
			}
		}
		if (!found_side)
			possible_intersecting_triangles.push_back(triangle);
	}

	std::vector<Triangle*> intersecting_triangles;

	for (const auto &triangle : possible_intersecting_triangles)
	{
		std::vector<OverlapInfo> Tinfos;
		for (int i = 0; i < triangle->verticies.size(); i++)
		{
			Point2D vec;
			if (i + 1 >= triangle->verticies.size())
				vec = triangle->verticies[0] - triangle->verticies[i];
			else
				vec = triangle->verticies[i + 1] - triangle->verticies[i];
			Point2D pvec = Point2D(-vec.y, vec.x);
			std::vector<float> projections;
			for (const auto &vertex : triangle->verticies)
			{
				projections.push_back(Dot2D(pvec, vertex));
			}
			OverlapInfo side_info;
			side_info.pvector = pvec;
			side_info.max = *std::max_element(std::begin(projections), std::end(projections));
			side_info.min = *std::min_element(std::begin(projections), std::end(projections));
			Tinfos.push_back(side_info);
		}

		bool found_side = false;
		for (const auto &side : Tinfos)
		{
			bool intersection = false;
			bool greater;
			if (Dot2D(polygon.points[0], side.pvector) > side.max)
				greater = true;
			else if (Dot2D(polygon.points[0], side.pvector) < side.min)
				greater = false;
			else
			{
				intersection = true;
				continue;
			}
			for (int i = 1; i < polygon.points.size(); i++)
			{
				if (Dot2D(polygon.points[i], side.pvector) <= side.max && greater)
				{
					intersection = true;
					break;
				}
				if (Dot2D(polygon.points[i], side.pvector) >= side.min && !greater)
				{
					intersection = true;
					break;
				}
			}
			if (!intersection)
			{
				found_side = true;
				break;
			}
		}
		if (!found_side)
			intersecting_triangles.push_back(triangle);
	}


	return intersecting_triangles;
}

std::vector<Point2D> NavMesh::FindIntersectionPoints(Polygon polygon, std::vector<Triangle*> overlapped_triangles)
{
	std::vector<Point2D> intersection_points;
	for (int i = 0; i < polygon.points.size(); i++)
	{
		int next = i + 1;
		if (next >= polygon.points.size())
			next = 0;
		for (const auto &triangle : overlapped_triangles)
		{
			Point2D intersection = FindLineSegmentIntersection(polygon.points[i], polygon.points[next], triangle->verticies[0], triangle->verticies[1]);
			if (intersection != Point2D(0, 0))
				intersection_points.push_back(intersection);
			intersection = FindLineSegmentIntersection(polygon.points[i], polygon.points[next], triangle->verticies[1], triangle->verticies[2]);
			if (intersection != Point2D(0, 0))
				intersection_points.push_back(intersection);
			intersection = FindLineSegmentIntersection(polygon.points[i], polygon.points[next], triangle->verticies[2], triangle->verticies[0]);
			if (intersection != Point2D(0, 0))
				intersection_points.push_back(intersection);
		}
	}
	for (int i = intersection_points.size() - 1; i >= 0; i--)
	{
		for (int j = i + 1; j < intersection_points.size(); j++)
		{
			if (Distance2D(intersection_points[i], intersection_points[j]) < .1)
			{
				intersection_points.erase(intersection_points.begin() + i);
				break;
			}
		}
	}
	return intersection_points;
}

Point2D NavMesh::FindLineSegmentIntersection(Point2D start1, Point2D end1, Point2D start2, Point2D end2)
{
	Point2D vec1 = end1 - start1;
	Point2D vec2 = end2 - start2;
	
	Point2D diff = start2 - start1;
	float cross = Cross(vec1, vec2);

	float coef1 = Cross(diff, vec2 / cross);
	float coef2 = Cross(diff, vec1 / cross);

	if (cross != 0 && coef1 >= 0 && coef1 <= 1 && coef2 >= 0 && coef2 <= 1)
	{
		return start1 + (vec1 * coef1);
	}
	else
	{
		return Point2D(0, 0);
	}
}

std::vector<Point2D> NavMesh::SaveVerticies(std::vector<Triangle*> triangles_to_remove)
{
	std::vector<Point2D> verticies;
	for (auto &triangle : triangles_to_remove)
	{
		for (const auto &vertex : triangle->verticies)
		{
			if (std::find(verticies.begin(), verticies.end(), vertex) == verticies.end())
				verticies.push_back(vertex);
		}
	}
	/*for (auto &triangle : triangles_to_remove)
	{
		for (auto &connection : triangle->connections)
		{
			if (std::find(triangles_to_remove.begin(), triangles_to_remove.end(), connection) == triangles_to_remove.end())
			{
				for (const auto &vertex : connection->verticies)
				{
					verticies.erase(std::remove(verticies.begin(), verticies.end(), vertex), verticies.end());
				}
			}
		}
	}*/
	return verticies;
}

void NavMesh::RemoveTriangles(std::vector<Triangle*> triangles_to_remove)
{
	for (auto &triangle : triangles_to_remove)
	{
		for (auto &connection : triangle->connections)
		{
			connection->connections.erase(std::remove(connection->connections.begin(), connection->connections.end(), triangle), connection->connections.end());
		}
		triangles.erase(std::remove(triangles.begin(), triangles.end(), triangle), triangles.end());
	}
}

void NavMesh::ReAddVerticies(std::vector<Point2D> removed_verticies, std::vector<Point2D> new_polygon)
{
	std::vector<Point2D> verticies = removed_verticies;
	verticies.insert(verticies.end(), new_polygon.begin(), new_polygon.end());

	float minX = INFINITY;
	float maxX = 0;
	float minY = INFINITY;
	float maxY = 0;
	for (const auto &vertex : verticies)
	{
		if (vertex.x < minX)
			minX = vertex.x;
		else if (vertex.x > maxX)
			maxX = vertex.x;
		if (vertex.y < minY)
			minY = vertex.y;
		else if (vertex.y > maxY)
			maxY = vertex.y;
	}
	minX--;
	maxX++;
	minY--;
	maxY++;

	// make supra-triangle
	Triangle* supra_triangle = new Triangle();
	supra_triangle->verticies.push_back(Point2D(minX, minY));
	supra_triangle->verticies.push_back(Point2D(minX, minY + (maxY - minY) * 2));
	supra_triangle->verticies.push_back(Point2D(minX + (maxX - minX) * 2, minY));

	std::vector<Triangle*> new_triangles;
	new_triangles.push_back(supra_triangle);

	// loop through points
	for (const auto &point : verticies)
	{
		std::vector<Triangle*> bad_triangles;
		//   loop through triangles
		for (auto &triangle : new_triangles)
		{
			//      compute circumcircle
			Circle circumcircle = ComputeCircumcircle(triangle);

			//      if point is within circle
			if (Distance2D(point, circumcircle.center) <= circumcircle.radius)
			{
				//          add triangle to bad triangles
				bad_triangles.push_back(triangle);
			}
		}
		std::vector<Vec2D> edges;
		//   for each triangle in bad triangles
		for (const auto &triangle : bad_triangles)
		{
			edges.push_back(Vec2D(triangle->verticies[0], triangle->verticies[1]));
			edges.push_back(Vec2D(triangle->verticies[1], triangle->verticies[2]));
			edges.push_back(Vec2D(triangle->verticies[2], triangle->verticies[0]));
			//      remove bad triangle from triangles
			new_triangles.erase(std::remove(new_triangles.begin(), new_triangles.end(), triangle), new_triangles.end());
		}
		// remove common edges
		for (int i = edges.size() - 1; i > 0; i--)
		{
			bool common = false;
			for (int j = i - 1; j >= 0; j--)
			{
				if (edges[i] == edges[j])
				{
					common = true;
					edges.erase(edges.begin() + j);
					i--;
				}
			}
			if (common)
				edges.erase(edges.begin() + i);
		}
		// make new triangles with edges
		for (const auto &edge : edges)
		{
			new_triangles.push_back(new Triangle({ point, edge.start, edge.end }));
		}
	}
	// delete any triangle with 2+ verticies the same as the supra triangle
	for (int i = new_triangles.size() - 1; i >= 0; i--)
	{
		if (DoesShareVertex(new_triangles[i], supra_triangle))
			new_triangles.erase(new_triangles.begin() + i);
	}

	for (const auto &triangle : new_triangles)
	{
		triangles.push_back(triangle);
	}
}

void NavMesh::AddVerticies(std::vector<Point2D> removed_verticies, std::vector<Point2D> new_polygon)
{
	std::vector<Point2D> verticies = removed_verticies;
	verticies.insert(verticies.end(), new_polygon.begin(), new_polygon.end());


	// loop through points
	for (const auto &point : verticies)
	{
		std::vector<Triangle*> bad_triangles;
		//   loop through triangles
		for (auto &triangle : triangles)
		{
			//      compute circumcircle
			Circle circumcircle = ComputeCircumcircle(triangle);

			//      if point is within circle
			if (Distance2D(point, circumcircle.center) <= circumcircle.radius)
			{
				//          add triangle to bad triangles
				bad_triangles.push_back(triangle);
			}
		}
		std::vector<Vec2D> edges;
		//   for each triangle in bad triangles
		for (const auto &triangle : bad_triangles)
		{
			edges.push_back(Vec2D(triangle->verticies[0], triangle->verticies[1]));
			edges.push_back(Vec2D(triangle->verticies[1], triangle->verticies[2]));
			edges.push_back(Vec2D(triangle->verticies[2], triangle->verticies[0]));
			//      remove bad triangle from triangles
			triangles.erase(std::remove(triangles.begin(), triangles.end(), triangle), triangles.end());
		}
		// remove common edges
		for (int i = edges.size() - 1; i > 0; i--)
		{
			bool common = false;
			for (int j = i - 1; j >= 0; j--)
			{
				if (edges[i] == edges[j])
				{
					common = true;
					edges.erase(edges.begin() + j);
					i--;
				}
			}
			if (common)
				edges.erase(edges.begin() + i);
		}
		// make new triangles with edges
		for (const auto &edge : edges)
		{
			triangles.push_back(new Triangle({ point, edge.start, edge.end }));
		}
	}
}

#pragma endregion



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

	if (tri_path.size() == 0)
	{
		std::vector<Point2D> path;
		path.push_back(end);
		return path;
	}

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
			if (!node->pathable)
				continue;
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

float NavMesh::Cross(Point2D vec1, Point2D vec2)
{
	return vec1.x * vec2.y - vec1.y * vec2.x;
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

void NavMesh::SaveNavMeshToFile(std::string filename)
{
	std::ofstream file;
	file.open(filename);
	for (const auto &triangle : triangles)
	{
		file << "T: ";
		file << triangle->center.x << ' ' << triangle->center.y << ' ';
		file << triangle->verticies[0].x << ' ' << triangle->verticies[0].y << ' ';
		file << triangle->verticies[1].x << ' ' << triangle->verticies[1].y << ' ';
		file << triangle->verticies[2].x << ' ' << triangle->verticies[2].y << ' ';
		file << triangle->pathable << '\n';
	}

	file << "###########\n";

	for (int i = 0; i < triangles.size(); i++)
	{
		file << "C: " << i << ' ';
		for (const auto &connection : triangles[i]->connections)
		{
			file << std::find(triangles.begin(), triangles.end(), connection) - triangles.begin() << ' ';
		}
		file << '\n';
	}
	file << "end";
	file.close();
}

bool NavMesh::BuildNavMeshFromFile(std::string filename)
{
	std::ifstream file;
	file.open(filename);
	if (!file)
		return false;

	std::string next;
	file >> next;
	while (next == "T:")
	{
		float cX, cY, v1x, v1y, v2x, v2y, v3x, v3y;
		bool pathable;
		file >> cX;
		file >> cY;
		file >> v1x;
		file >> v1y;
		file >> v2x;
		file >> v2y;
		file >> v3x;
		file >> v3y;
		file >> pathable;
		Triangle* triangle = new Triangle();
		triangle->center = Point2D(cX, cY);
		triangle->verticies.push_back(Point2D(v1x, v1y));
		triangle->verticies.push_back(Point2D(v2x, v2y));
		triangle->verticies.push_back(Point2D(v3x, v3y));
		triangle->pathable = pathable;
		triangles.push_back(triangle);
		file >> next;
	}
	file >> next;
	while (next == "C:")
	{
		int index;
		file >> index;
		file >> next;
		while (next != "C:" && next != "end")
		{
			triangles[index]->connections.push_back(triangles[std::stoi(next)]);
			file >> next;
		}
	}
	return true;

}


void NavMesh::MakeSeparateTriangles(std::vector<Point2D> verticies, ImageData map)
{
	float section_width = map.width / sections;
	float section_height = map.height / sections;

	for (int i = 0; i < sections; i++)
	{
		for (int j = 0; j < sections; j++)
		{
			float left = section_width * i;
			float right = section_width * (i + 1);
			float bottom = section_height * j;
			float top = section_height * (j + 1);
			Triangle* tri1 = new Triangle({ Point2D(left, bottom), Point2D(right, bottom),  Point2D(left, top) });
			Triangle* tri2 = new Triangle({ Point2D(right, top), Point2D(right, bottom),  Point2D(left, top) });
			std::vector<Triangle*> *section_triangles = &separated_triangles[i][j];
			section_triangles->push_back(tri1);
			section_triangles->push_back(tri2);
			
			std::vector<Point2D> section_verticies;
			for (const auto &vertex : verticies)
			{
				if (vertex.x < left || vertex.x > right || vertex.y < bottom || vertex.y > top)
					continue;
				section_verticies.push_back(vertex);
				// TODO remove vertex from main list
			}
			for (const auto &point : section_verticies)
			{
				std::vector<Triangle*> bad_triangles;
				//   loop through triangles
				for (const auto &triangle : *section_triangles)
				{
					//      compute circumcircle
					Circle circumcircle = ComputeCircumcircle(triangle);

					//      if point is within circle
					if (Distance2D(point, circumcircle.center) <= circumcircle.radius)
					{
						//          add triangle to bad triangles
						bad_triangles.push_back(triangle);
					}
				}
				std::vector<Vec2D> edges;
				//   for each triangle in bad triangles
				for (const auto &triangle : bad_triangles)
				{
					edges.push_back(Vec2D(triangle->verticies[0], triangle->verticies[1]));
					edges.push_back(Vec2D(triangle->verticies[1], triangle->verticies[2]));
					edges.push_back(Vec2D(triangle->verticies[2], triangle->verticies[0]));
					//      remove bad triangle from section_triangles
					section_triangles->erase(std::remove(section_triangles->begin(), section_triangles->end(), triangle), section_triangles->end());
				}
				// remove common edges
				for (int i = edges.size() - 1; i > 0; i--)
				{
					bool common = false;
					for (int j = i - 1; j >= 0; j--)
					{
						if (edges[i] == edges[j])
						{
							common = true;
							edges.erase(edges.begin() + j);
							i--;
						}
					}
					if (common)
						edges.erase(edges.begin() + i);
				}
				// make new triangles with edges
				for (const auto &edge : edges)
				{
					section_triangles->push_back(new Triangle({ point, edge.start, edge.end }));
				}
			}

		}
	}
	
}

std::vector<Point2D> NavMesh::GetAllVerticiesWithIntersections(std::vector<Polygon> polygons, ImageData map)
{
	float section_width = map.width / sections;
	float section_height = map.height / sections;
	std::vector<Vec2D> divisions;

	for (int i = 0; i < sections; i++)
	{
		divisions.push_back(Vec2D(Point2D(0, section_height * i), Point2D(map.width, section_height * i)));
		divisions.push_back(Vec2D(Point2D(section_width * i, 0), Point2D(section_width * i, map.height)));
	}
	std::vector<Point2D> points;
	for (const auto &polygon : polygons)
	{
		for (int i = 0; i < polygon.points.size(); i++)
		{
			Point2D current_point = polygon.points[i];
			Point2D next_point;
			if (i + 1 >= polygon.points.size())
				next_point = polygon.points[0];
			else
				next_point = polygon.points[i + 1];

			points.push_back(current_point);
			for (const auto &division : divisions)
			{
				Point2D intersection = FindLineSegmentIntersection(current_point, next_point, division.start, division.end);
				if (intersection != Point2D(0, 0))
					points.push_back(intersection);
			}

		}
	}
	return points;
}

}