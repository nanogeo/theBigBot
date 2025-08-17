
#include <stdlib.h>

#include "point_path.h"


namespace sc2
{

Point2D PointPath::FindClosestPoint(Point2D point) const
{
	Point2D closest_point = Point2D(0, 0);
	float dist = 0;
	for (const auto& pos : points)
	{
		if (closest_point == Point2D(0, 0) || Distance2D(pos, point) < dist)
		{
			closest_point = pos;
			dist = Distance2D(pos, point);
		}
	}
	return closest_point;
}

Point2D PointPath::GetPointFrom(Point2D point, float dist, bool forward) const
{
	if (forward)
	{
		for (int i = points.size() - 1; i >= 0; i--)
		{
			if (Distance2D(point, points[i]) < dist)
			{
				if (points[i] == point && i < points.size() - 1)
				{
					float dist_to_next = Distance2D(points[i], points[i + 1]);
					if (dist / dist_to_next > .5)
						return points[i + 1];
				}
				return points[i];
			}
		}
	}
	else
	{
		for (int i = 0; i < points.size(); i++)
		{
			if (Distance2D(point, points[i]) < dist)
			{
				if (points[i] == point && i > 0)
				{
					float dist_to_next = Distance2D(points[i], points[i - 1]);
					if (dist / dist_to_next > .5)
						return points[i - 1];
				}
				return points[i];
			}
		}
	}
}

std::vector<Point2D> PointPath::FindCircleIntersection(Point2D point, float radius) const
{
	std::vector<Point2D> intersection_points;
	int prev_position = 0;
	for (int i = 0; i < points.size(); i++)
	{
		if (prev_position == 0)
		{
			if (Distance2D(point, points[i]) < radius)
				prev_position = -1;
			else
				prev_position = 1;
		}
		else if ((prev_position == -1 && Distance2D(point, points[i]) > radius) ||
			(prev_position == 1 && Distance2D(point, points[i]) < radius))
		{
			if (abs(Distance2D(point, points[i]) - radius) < abs(Distance2D(point, points[i - 1]) - radius))
				intersection_points.push_back(points[i]);
			else
				intersection_points.push_back(points[i - 1]);

			prev_position *= -1;
		}
	}
	return intersection_points;
}

std::vector<Point2D> PointPath::GetPoints() const
{
	std::vector<Point2D> points;
	for (const auto& node : points)
	{
		points.push_back(node);
	}
	return points;
}

Point2D PointPath::GetStartPoint() const
{
	return points[0];
}

Point2D PointPath::GetEndPoint() const
{
	return points.back();
}

Point2D PointPath::GetFurthestForward(std::vector<Point2D> points) const
{
	for (int i = points.size() - 1; i >= 0; i--)
	{
		if (std::find(points.begin(), points.end(), points[i]) != points.end())
			return points[i];
	}
	// Error no points in path
	return Point2D(0, 0);
}

Point2D PointPath::GetFurthestBack(std::vector<Point2D> points) const
{
	for (int i = 0; i < points.size(); i++)
	{
		if (std::find(points.begin(), points.end(), points[i]) != points.end())
			return points[i];
	}
	// Error no points in path
	return Point2D(0, 0);
}

}