
#include "TossBot.h"
#include "path_manager.h"


constexpr double PI = 3.14159265358979323;

namespace sc2
{

Point2D LineSegmentLinearX::FindClosestPoint(Point2D point)
{
	double perp_slope = - 1 / a;
	double perp_coef = point.y - point.x * perp_slope;
	double coef = b;

	Point2D closest_pos = Point2D((perp_coef - coef) / (a - perp_slope), a * ((perp_coef - coef) / (a - perp_slope)) + coef);

	if (closest_pos.x < min)
		return Point2D(min, a * min + b);
	if (closest_pos.x > max)
		return Point2D(max, a * max + b);
	return closest_pos;
}

Point2D LineSegmentLinearY::FindClosestPoint(Point2D point)
{
	double perp_slope = -1 / a;
	double perp_coef = point.x - point.y * perp_slope;
	double coef = b;

	Point2D closest_pos = Point2D(a * ((perp_coef - coef) / (a - perp_slope)) + coef, (perp_coef - coef) / (a - perp_slope));

	if (closest_pos.y < min)
		return Point2D(a * min + b, min);
	if (closest_pos.y > max)
		return Point2D(a * max + b, max);
	return closest_pos;
}

Point2D LineSegmentCurveX::FindClosestPoint(Point2D point)
{
	// derivative of distance
	double a1 = 4 * pow(a, 2);
	double b1 = 6 * a * b;
	double c1 = 2 + 2 * pow(b, 2) + 4 * a * c - 4 * a * point.y;
	double d1 = 2 * b * c - 2 * point.x - 2 * b * point.y;

	// cubic formula
	double p = (c1 / a1) - (pow(b1, 2) / (3 * pow(a1, 2)));
	double q = ((2 * pow(b1, 3)) / (27 * pow(a1, 3))) - ((b1 * c1) / (3 * pow(a1, 2))) + (d1 / a1);

	double num_roots = pow(q / 2, 2) + pow(p / 3, 3);

	if (num_roots > 0)
	{
		// only 1 solutions
		double x_value = std::cbrt(-1 * (q / 2) - std::sqrt(num_roots)) + std::cbrt(-1 * (q / 2) + std::sqrt(num_roots)) - (b1 / (3 * a1));
		if (x_value < min)
			return Point2D(min, a * pow(min, 2) + b * min + c);
		if (x_value > max)
			return Point2D(max, a * pow(max, 2) + b * max + c);
		else
			return Point2D(x_value, a * pow(x_value, 2) + b * x_value + c);
	}
	if (num_roots < 0)
	{
		// 3 solutions
		double r = -1 * q / 2;
		double i = -1 * sqrt(abs(num_roots));
		double dist = sqrt(pow(r, 2) + pow(i, 2));
		double angle = atan2(i, r);

		double s1 = 2 * std::cbrt(dist) * cos(angle / 3) - (b1 / (3 * a1));
		double s2 = 2 * std::cbrt(dist) * cos((angle + 2 * PI) / 3) - (b1 / (3 * a1));
		double s3 = 2 * std::cbrt(dist) * cos((angle + 4 * PI) / 3) - (b1 / (3 * a1));

		Point2D p1 = Point2D(s1, a * pow(s1, 2) + b * s1 + c);
		Point2D p2 = Point2D(s2, a * pow(s2, 2) + b * s2 + c);
		Point2D p3 = Point2D(s3, a * pow(s3, 2) + b * s3 + c);

		double dist1 = Distance2D(point, p1);
		double dist2 = Distance2D(point, p2);
		double dist3 = Distance2D(point, p3);

		if (dist1 <= dist2 && dist1 <= dist3)
			return p1;
		if (dist2 <= dist3)
			return p2;
		else
			return p3;
	}
	else
	{
		// 2 solutions
		// very very unlikely
		std::cout << "2 solutions in LineSegmentCurveX::FindClosestPoint";
		return point;
	}
}

Point2D LineSegmentCurveY::FindClosestPoint(Point2D point)
{
	// derivative of distance
	double a1 = 4 * pow(a, 2);
	double b1 = 6 * a * b;
	double c1 = 2 + 2 * pow(b, 2) + 4 * a * c - 4 * a * point.x;
	double d1 = 2 * b * c - 2 * point.y - 2 * b * point.x;

	// cubic formula
	double p = (c1 / a1) - (pow(b1, 2) / (3 * pow(a1, 2)));
	double q = ((2 * pow(b1, 3)) / (27 * pow(a1, 3))) - ((b1 * c1) / (3 * pow(a1, 2))) + (d1 / a1);

	double num_roots = pow(q / 2, 2) + pow(p / 3, 3);

	if (num_roots > 0)
	{
		// only 1 solutions
		double y_value = std::cbrt(-1 * (q / 2) - std::sqrt(num_roots)) + std::cbrt(-1 * (q / 2) + std::sqrt(num_roots)) - (b1 / (3 * a1));
		if (y_value < min)
			return Point2D(a * pow(min, 2) + b * min + c, min);
		if (y_value > max)
			return Point2D(a * pow(max, 2) + b * max + c, max);
		else
			return Point2D( a * pow(y_value, 2) + b * y_value + c,y_value);
	}
	if (num_roots < 0)
	{
		// 3 solutions
		double r = -1 * q / 2;
		double i = -1 * sqrt(abs(num_roots));
		double dist = sqrt(pow(r, 2) + pow(i, 2));
		double angle = atan2(i, r);

		double s1 = 2 * std::cbrt(dist) * cos(angle / 3) - (b1 / (3 * a1));
		double s2 = 2 * std::cbrt(dist) * cos((angle + 2 * PI) / 3) - (b1 / (3 * a1));
		double s3 = 2 * std::cbrt(dist) * cos((angle + 4 * PI) / 3) - (b1 / (3 * a1));

		Point2D p1 = Point2D(a * pow(s1, 2) + b * s1 + c, s1);
		Point2D p2 = Point2D(a * pow(s2, 2) + b * s2 + c, s2);
		Point2D p3 = Point2D(a * pow(s3, 2) + b * s3 + c, s3);

		double dist1 = Distance2D(point, p1);
		double dist2 = Distance2D(point, p2);
		double dist3 = Distance2D(point, p3);

		if (dist1 <= dist2 && dist1 <= dist3)
			return p1;
		if (dist2 <= dist3)
			return p2;
		else
			return p3;
	}
	else
	{
		// 2 solutions
		// very very unlikely
		std::cout << "2 solutions in LineSegmentCurveY::FindClosestPoint";
		return point;
	}
}

std::vector<Point2D> LineSegmentLinearX::FindCircleIntersection(Point2D center, double radius)
{
	double A = pow(a, 2) + 1;
	double B = (2 * a * b) - (2 * center.x) - (2 * a * center.y);
	double C = pow(center.x, 2) + pow(b, 2) + pow(center.y, 2) - (2 * b * center.y) - pow(radius, 2);

	double deter = pow(B, 2) - (4 * A * C);

	if (deter < 0)
		return std::vector<Point2D>();
	else if (deter == 0)
	{
		double x_val = (-1 * B) / (2 * A);
		std::vector<Point2D> solutions;
		if (x_val >= min && x_val <= max)
			solutions.push_back(EvaluateAt(x_val));
		return solutions;
	}
	else
	{
		double x_val1 = (-1 * B + sqrt(deter)) / (2 * A);
		double x_val2 = (-1 * B - sqrt(deter)) / (2 * A);
		std::vector<Point2D> solutions;
		if (x_val1 >= min && x_val1 <= max)
			solutions.push_back(EvaluateAt(x_val1));
		if (x_val2 >= min && x_val2 <= max)
			solutions.push_back(EvaluateAt(x_val2));
		return solutions;
	}
}

std::vector<Point2D> LineSegmentLinearY::FindCircleIntersection(Point2D center, double radius)
{
	double A = pow(a, 2) + 1;
	double B = (2 * a * b) - (2 * center.y) - (2 * a * center.x);
	double C = pow(center.y, 2) + pow(b, 2) + pow(center.x, 2) - (2 * b * center.x) - pow(radius, 2);

	double deter = pow(B, 2) - (4 * A * C);

	if (deter < 0)
		return std::vector<Point2D>();
	else if (deter == 0)
	{
		double y_val = (-1 * B) / (2 * A);
		std::vector<Point2D> solutions;
		if (y_val >= min && y_val <= max)
			solutions.push_back(EvaluateAt(y_val));
		return solutions;
	}
	else
	{
		double y_val1 = (-1 * B + sqrt(deter)) / (2 * A);
		double y_val2 = (-1 * B - sqrt(deter)) / (2 * A);
		std::vector<Point2D> solutions;
		if (y_val1 >= min && y_val1 <= max)
			solutions.push_back(EvaluateAt(y_val1));
		if (y_val2 >= min && y_val2 <= max)
			solutions.push_back(EvaluateAt(y_val2));
		return solutions;
	}
}

std::vector<Point2D> LineSegmentCurveX::FindCircleIntersection(Point2D center, double radius)
{
	std::vector<Point2D> solutions;
	// combine equations to get quartic
	double A = pow(a, 2);
	double B = 2 * a * b;
	double C = (2 * a * c) - (2 * a * center.y) + pow(b, 2) + 1;
	double D = (2 * b * c) - (2 * b * center.y) - (2 * center.x);
	double E = pow(c, 2) - (2 * c * center.y) + pow(center.y, 2) + pow(center.x, 2) - pow(radius, 2);

	// quartic formula
	double p1 = (2 * pow(C, 3)) - (9 * B * C * D) + (27 * A * pow(D, 2)) + (27 * pow(B, 2) * E) - (72 * A * C *E);
	double inter1 = pow(C, 2) - (3 * B * D) + (12 * A * E);
	double any_solution = -4 * pow(inter1, 3) + pow(p1, 2);
	if (any_solution < 0)
		return solutions;
	double p2 = p1 + sqrt(any_solution);
	double inter2 = cbrt(p2 / 2);
	double p3 = (inter1 / (3 * A * inter2)) + (inter2 / (3 * A));
	double inter3 = (pow(B, 2) / (4 * pow(A, 2))) - ((2 * C) / (3 * A));
	double sqrt_check = inter3 + p3;
	if (sqrt_check < 0)
		return solutions;
	double p4 = sqrt(sqrt_check);
	double p5 = (pow(B, 2) / (2 * pow(A, 2))) - ((4 * C) / (3 * A)) - p3;
	double p6 = ((-1 * pow(B, 3) / pow(A, 3)) + ((4 * B * C) / pow(A, 2)) - ((8 * D) / A)) / (4 * p4);

	double inter4 = ((-1 * B) / (4 * A)) - (p4 / 2);
	double deter1 = p5 - p6;
	if (deter1 >= 0)
	{
		double inter5 = sqrt(deter1) / 2;
		double solution1 = inter4 - inter5;
		double solution2 = inter4 + inter5;
		if (solution1 >= min && solution1 <= max)
			solutions.push_back(EvaluateAt(solution1));
		if (solution2 >= min && solution2 <= max)
			solutions.push_back(EvaluateAt(solution2));
	}

	inter4 = ((-1 * B) / (4 * A)) + (p4 / 2);
	double deter2 = p5 + p6;
	if (deter2 >= 0)
	{
		double inter5 = sqrt(deter2) / 2;
		double solution3 = inter4 - inter5;
		double solution4 = inter4 + inter5;
		if (solution3 >= min && solution3 <= max)
			solutions.push_back(EvaluateAt(solution3));
		if (solution4 >= min && solution4 <= max)
			solutions.push_back(EvaluateAt(solution4));
	}
	return solutions;
}

std::vector<Point2D> LineSegmentCurveY::FindCircleIntersection(Point2D center, double radius)
{
	std::vector<Point2D> solutions;
	// combine equations to get quartic
	double A = pow(a, 2);
	double B = 2 * a * b;
	double C = (2 * a * c) - (2 * a * center.x) + pow(b, 2) + 1;
	double D = (2 * b * c) - (2 * b * center.x) - (2 * center.y);
	double E = pow(c, 2) - (2 * c * center.x) + pow(center.x, 2) + pow(center.y, 2) - pow(radius, 2);

	// quartic formula
	double p1 = (2 * pow(C, 3)) - (9 * B * C * D) + (27 * A * pow(D, 2)) + (27 * pow(B, 2) * E) - (72 * A * C *E);
	double inter1 = pow(C, 2) - (3 * B * D) + (12 * A * E);
	double any_solution = -4 * pow(inter1, 3) + pow(p1, 2);
	if (any_solution < 0)
		return solutions;
	double p2 = p1 + sqrt(any_solution);
	double inter2 = cbrt(p2 / 2);
	double p3 = (inter1 / (3 * A * inter2)) + (inter2 / (3 * A));
	double inter3 = (pow(B, 2) / (4 * pow(A, 2))) - ((2 * C) / (3 * A));
	double sqrt_check = inter3 + p3;
	if (sqrt_check < 0)
		return solutions;
	double p4 = sqrt(sqrt_check);
	double p5 = (pow(B, 2) / (2 * pow(A, 2))) - ((4 * C) / (3 * A)) - p3;
	double p6 = ((-1 * pow(B, 3) / pow(A, 3)) + ((4 * B * C) / pow(A, 2)) - ((8 * D) / A)) / (4 * p4);

	double inter4 = ((-1 * B) / (4 * A)) - (p4 / 2);
	double deter1 = p5 - p6;
	if (deter1 >= 0)
	{
		double inter5 = sqrt(deter1) / 2;
		double solution1 = inter4 - inter5;
		double solution2 = inter4 + inter5;
		if (solution1 >= min && solution1 <= max)
			solutions.push_back(EvaluateAt(solution1));
		if (solution2 >= min && solution2 <= max)
			solutions.push_back(EvaluateAt(solution2));
	}

	inter4 = ((-1 * B) / (4 * A)) + (p4 / 2);
	double deter2 = p5 + p6;
	if (deter2 >= 0)
	{
		double inter5 = sqrt(deter2) / 2;
		double solution3 = inter4 - inter5;
		double solution4 = inter4 + inter5;
		if (solution3 >= min && solution3 <= max)
			solutions.push_back(EvaluateAt(solution3));
		if (solution4 >= min && solution4 <= max)
			solutions.push_back(EvaluateAt(solution4));
	}
	return solutions;
}


Point2D PathManager::FindClosestPoint(Point2D point)
{
	Point2D closest;
	double distance_to_closest = INFINITY;

	for (auto &segment : segments)
	{
		Point2D closest_on_segment = segment->FindClosestPoint(point);
		double dist = Distance2D(point, closest_on_segment);
		if (dist < distance_to_closest)
		{
			distance_to_closest = dist;
			closest = closest_on_segment;
		}
	}
	return closest;
}

std::vector<Point2D> PathManager::FindCircleIntersection(Point2D center, double radius)
{
	std::vector<Point2D> intersections;
	for (const auto &segment : segments)
	{
		std::vector<Point2D> line_intersections = segment->FindCircleIntersection(center, radius);
		if (line_intersections.size() > 0)
			intersections.insert(intersections.end(), line_intersections.begin(), line_intersections.end());
	}
	return intersections;
}

Point2D PathManager::GetPointFrom(Point2D point, double dist, bool forward)
{
	std::vector<Point2D> circle_intersections = FindCircleIntersection(point, dist);
	if (circle_intersections.size() == 0)
	{
		std::cout << "Error no intersections found in PathManager::GetPointFrom\n";
		circle_intersections = FindCircleIntersection(point, dist);
		return Point2D(0, 0);
	}
	if (circle_intersections.size() > 2)
	{
		for (int i = 0; i < circle_intersections.size() - 1; i++)
		{
			Point2D p1 = circle_intersections[i];
			for (int j = 1; j < circle_intersections.size(); j++)
			{
				Point2D p2 = circle_intersections[j];
				std::cout << "check for dupe points " << p1.x << ", " << p1.y << " - " << p2.x << ", " << p2.y << "\n";
				if (p1.x + 1 > p2.x && p1.x - 1 < p2.x && p1.y + 1 > p2.y && p1.y - 1 < p2.y)
				{
					circle_intersections.erase(circle_intersections.begin() + j);
					j--;
					std::cout << "removing " << p2.x << ", " << p2.y << "\n";
					if (circle_intersections.size() <= 2)
						break;
				}
			}
			if (circle_intersections.size() <= 2)
				break;
		}
	}

	if (circle_intersections.size() == 1)
	{
		Point2D end_point;
		if (pos_direction && forward)
			end_point = segments[segments.size() - 1]->EvaluateAt(segments[segments.size() - 1]->GetMax());
		else if (pos_direction && !forward)
			end_point = segments[0]->EvaluateAt(segments[0]->GetMin());
		else if (!pos_direction && forward)
			end_point = segments[segments.size() - 1]->EvaluateAt(segments[segments.size() - 1]->GetMin());
		else if (!pos_direction && !forward)
			end_point = segments[0]->EvaluateAt(segments[0]->GetMax());

		if (Distance2D(end_point, point) > dist)
			return circle_intersections[0];

		if ((pos_direction && forward) || (!pos_direction && !forward))
		{
			if ((x_based && circle_intersections[0].x > end_point.x) || (!x_based && circle_intersections[0].y > end_point.y))
				return circle_intersections[0];
			else
				return end_point;
		}
		else
		{
			if ((x_based && circle_intersections[0].x < end_point.x) || (!x_based && circle_intersections[0].y < end_point.y))
				return circle_intersections[0];
			else
				return end_point;
		}
	}
	else if (circle_intersections.size() == 2)
	{
		if ((pos_direction && forward) || (!pos_direction && !forward))
		{
			if ((x_based && circle_intersections[0].x > circle_intersections[1].x) || (!x_based && circle_intersections[0].y > circle_intersections[1].y))
				return circle_intersections[0];
			else
				return circle_intersections[1];
		}
		else
		{
			if ((x_based && circle_intersections[0].x < circle_intersections[1].x) || (!x_based && circle_intersections[0].y < circle_intersections[1].y))
				return circle_intersections[0];
			else
				return circle_intersections[1];
		}
	}


}

std::vector<Point2D> PathManager::GetPoints()
{
	Point2D current;
	if (pos_direction)
		current = segments[0]->EvaluateAt(segments[0]->GetMin());
	else
		current = segments[0]->EvaluateAt(segments[0]->GetMax());

	std::vector<Point2D> points;
	while (true)
	{
		if (x_based)
		{
			if (pos_direction)
			{
				if (current.x >= segments[segments.size() - 1]->EvaluateAt(segments[segments.size() - 1]->GetMax()).x)
				{
					break;
				}
			}
			else
			{
				if (current.x <= segments[segments.size() - 1]->EvaluateAt(segments[segments.size() - 1]->GetMin()).x)
				{
					break;
				}
			}
		}
		else
		{
			if (pos_direction)
			{
				if (current.y >= segments[segments.size() - 1]->EvaluateAt(segments[segments.size() - 1]->GetMax()).y)
				{
					break;
				}
			}
			else
			{
				if (current.y <= segments[segments.size() - 1]->EvaluateAt(segments[segments.size() - 1]->GetMin()).y)
				{
					break;
				}
			}
		}

		points.push_back(current);
		current = GetPointFrom(current, 1, true);
	}

	/*int start;
	if (pos_direction)
		start = ceil(segments[0]->GetMin());
	else
		start = ceil(segments[0]->GetMax());

	std::vector<Point2D> points;
	int curr = start;
	for (const auto &segment : segments)
	{
		if (pos_direction)
		{
			while (curr <= segment->GetMax())
			{
				points.push_back(segment->EvaluateAt(curr));
				curr++;
			}
		}
		else
		{
			while (curr >= segment->GetMin())
			{
				points.push_back(segment->EvaluateAt(curr));
				curr--;
			}
		}
	}*/
	return points;
}



}