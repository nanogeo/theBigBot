
#include <stdlib.h>

#include "theBigBot.h"
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
		//std::cout << "2 solutions in LineSegmentCurveX::FindClosestPoint";
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
		//std::cout << "2 solutions in LineSegmentCurveY::FindClosestPoint";
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

	std::vector<double> roots = Utility::GetRealQuarticRoots(A, B, C, D, E);
	for (const auto& root : roots)
	{
		if (root >= min && root <= max)
			solutions.push_back(EvaluateAt(root));
	}

	// quartic formula
	/*double p1 = (2 * pow(C, 3)) - (9 * B * C * D) + (27 * A * pow(D, 2)) + (27 * pow(B, 2) * E) - (72 * A * C *E);
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
	}*/
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

	std::vector<double> roots = Utility::GetRealQuarticRoots(A, B, C, D, E);
	for (const auto& root : roots)
	{
		if (root >= min && root <= max)
			solutions.push_back(EvaluateAt(root));
	}

	// quartic formula
	/*double p1 = (2 * pow(C, 3)) - (9 * B * C * D) + (27 * A * pow(D, 2)) + (27 * pow(B, 2) * E) - (72 * A * C *E);
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
	}*/
	return solutions;
}


Point2D LineSegmentLinearX::GetPointFrom(Point2D point, double dist, bool forward, double& dist_left)
{
	bool direction = forward == pos_direction;
	if (!direction)
		dist *= -1;

	double new_pos = point.x + dist;
	if (new_pos < min)
	{
		dist_left = abs(min - new_pos);
		return EvaluateAt(min);
	}
	if (new_pos > max)
	{
		dist_left = abs(new_pos - max);
		return EvaluateAt(max);
	}
	
	return EvaluateAt(new_pos);
}

Point2D LineSegmentLinearY::GetPointFrom(Point2D point, double dist, bool forward, double& dist_left)
{
	bool direction = forward == pos_direction;
	if (!direction)
		dist *= -1;

	double new_pos = point.y + dist;
	if (new_pos < min)
	{
		dist_left = abs(min - new_pos);
		return EvaluateAt(min);
	}
	if (new_pos > max)
	{
		dist_left = abs(new_pos - max);
		return EvaluateAt(max);
	}

	return EvaluateAt(new_pos);
}

Point2D LineSegmentCurveX::GetPointFrom(Point2D point, double dist, bool forward, double& dist_left)
{
	bool direction = forward == pos_direction;
	if (!direction)
		dist *= -1;

	double new_pos = point.x + dist;
	if (new_pos < min)
	{
		dist_left = abs(min - new_pos);
		return EvaluateAt(min);
	}
	if (new_pos > max)
	{
		dist_left = abs(new_pos - max);
		return EvaluateAt(max);
	}

	return EvaluateAt(new_pos);
}

Point2D LineSegmentCurveY::GetPointFrom(Point2D point, double dist, bool forward, double& dist_left)
{
	bool direction = forward == pos_direction;
	if (!direction)
		dist *= -1;

	double new_pos = point.y + dist;
	if (new_pos < min)
	{
		dist_left = abs(min - new_pos);
		return EvaluateAt(min);
	}
	if (new_pos > max)
	{
		dist_left = abs(new_pos - max);
		return EvaluateAt(max);
	}

	return EvaluateAt(new_pos);
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

int PathManager::FindClosestSegmentIndex(Point2D point)
{
	int closest;
	double distance_to_closest = INFINITY;

	for (int i = 0; i < segments.size(); i++)
	{
		Point2D closest_on_segment = segments[i]->FindClosestPoint(point);
		double dist = Distance2D(point, closest_on_segment);
		if (dist < distance_to_closest)
		{
			distance_to_closest = dist;
			closest = i;
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
	int index = FindClosestSegmentIndex(point);
	double dist_left = 0;
	Point2D new_point = segments[index]->GetPointFrom(point, dist, forward, dist_left);

	if (dist_left > 0)
	{
		int next_segment_index = forward ? index + 1 : index - 1;
		if (next_segment_index >= segments.size() || next_segment_index < 0)
			return forward ? segments[index]->GetEndPoint() : segments[index]->GetStartPoint();
		else
			return segments[next_segment_index]->GetPointFrom(point, dist_left, forward, dist);
	}
	else
	{
		return new_point;
	}
}

std::vector<Point2D> PathManager::GetPoints()
{
	Point2D current = segments[0]->GetStartPoint();

	std::vector<Point2D> points;
	for (const auto* segment : segments)
	{
		std::vector<Point2D> new_points = segment->GetPoints();
		points.insert(points.end(), new_points.begin(), new_points.end());
	}
	return points;
}

Point2D PathManager::GetStartPoint()
{
	return segments[0]->GetStartPoint();
}

Point2D PathManager::GetEndPoint()
{
	return segments[segments.size() - 1]->GetEndPoint();
}


LineSegment* PathManager::FitLineSegment(Point2D p1, Point2D p2, Point2D p3, Point2D p4)
{

	double curve_x_a, curve_x_b, curve_x_c, curve_x_loss;

	{
		double slope1 = (p1.y - p3.y) / (p1.x - p3.x);
		double slope2 = (p2.y - p4.y) / (p2.x - p4.x);

		#define d 2 * p1.x
		#define f (p1.y - p2.y) / (p1.x - p2.x)
		#define g - (p1.x * p1.x - p2.x * p2.x) / (p1.x - p2.x)
		#define h - slope1
		#define j 2 * p2.x
		#define k - slope2

		double df = d * f;
		double dh = d * h;
		double fg = 2 * f * g;
		double gh = g * h;
		double jf = j * f;
		double jk = j * k;
		double gk = g * k;
		double dd = d * d;
		double dg = 2 * d * g;
		double gg = g * g;
		double jj = j * j;
		double jg = 2 * j * g;



		double top = (d * f + d * h + 2 * f * g + g * h + j * f + j * k + g * k);
		double bot = (d * d + 2 * d * g + 2 * g * g + j * j + 2 * j * g);

		double a = -(d * f + d * h + 2 * f * g + g * h + j * f + j * k + g * k) / (d * d + 2 * d * g + 2 * g * g + j * j + 2 * j * g);
		double b = ((p1.y - p2.y) - a * (p1.x * p1.x - p2.x * p2.x)) / (p1.x - p2.x);
		double c = p1.y - a * p1.x * p1.x - b * p1.x;

		curve_x_a = a;
		curve_x_b = b;
		curve_x_c = c;

		curve_x_loss = (d * a + f + g * a + h) * (d * a + f + g * a + h) + (j * a + f + g * a + k) * (j * a + f + g * a + k);
		if (curve_x_loss != curve_x_loss)
			curve_x_loss = INFINITY;
	}

	double curve_y_a, curve_y_b, curve_y_c, curve_y_loss;

	{
		double slope1 = (p1.x - p3.x) / (p1.y - p3.y);
		double slope2 = (p2.x - p4.x) / (p2.y - p4.y);

		#define d 2 * p1.y
		#define f (p1.x - p2.x) / (p1.y - p2.y)
		#define g - (p1.y * p1.y - p2.y * p2.y) / (p1.y - p2.y)
		#define h - slope1
		#define j 2 * p2.y
		#define k - slope2

		double df = d * f;
		double dh = d * h;
		double fg = 2 * f * g;
		double gh = g * h;
		double jf = j * f;
		double jk = j * k;
		double gk = g * k;
		double dd = d * d;
		double dg = 2 * d * g;
		double gg = g * g;
		double jj = j * j;
		double jg = 2 * j * g;



		double top = (d * f + d * h + 2 * f * g + g * h + j * f + j * k + g * k);
		double bot = (d * d + 2 * d * g + 2 * g * g + j * j + 2 * j * g);

		double a = -(d * f + d * h + 2 * f * g + g * h + j * f + j * k + g * k) / (d * d + 2 * d * g + 2 * g * g + j * j + 2 * j * g);
		double b = ((p1.x - p2.x) - a * (p1.y * p1.y - p2.y * p2.y)) / (p1.y - p2.y);
		double c = p1.x - a * p1.y * p1.y - b * p1.y;

		curve_y_a = a;
		curve_y_b = b;
		curve_y_c = c;

		curve_y_loss = (d * a + f + g * a + h) * (d * a + f + g * a + h) + (j * a + f + g * a + k) * (j * a + f + g * a + k);
		if (curve_y_loss != curve_y_loss)
			curve_y_loss = INFINITY;
	}

	double line_x_a, line_x_b, line_x_loss;

	{
		double slope1 = (p1.y - p3.y) / (p1.x - p3.x);
		double slope2 = (p2.y - p4.y) / (p2.x - p4.x);

		double slope = (p1.y - p2.y) / (p1.x - p2.x);

		line_x_a = slope;
		line_x_b = p1.y - (slope * p1.x);

		line_x_loss = (slope - slope1) * (slope - slope1) + (slope - slope2) * (slope - slope2);
	}

	double line_y_a, line_y_b, line_y_loss;

	{
		double slope1 = (p1.x - p3.x) / (p1.y - p3.y);
		double slope2 = (p2.x - p4.x) / (p2.y - p4.y);

		double slope = (p1.x - p2.x) / (p1.y - p2.y);

		line_y_a = slope;
		line_y_b = p1.x - (slope * p1.y);

		line_y_loss = (slope - slope1) * (slope - slope1) + (slope - slope2) * (slope - slope2);
	}




	if (curve_x_loss < curve_y_loss)
	{
		if (curve_x_loss < line_x_loss)
		{
			return new LineSegmentCurveX(curve_x_a, curve_x_b, curve_x_c, p1.x, p2.x, false, Point2D(0, 0), false);
		}
		else
		{
			if (abs(p1.x - p2.x) > abs(p1.y - p2.y))
			{
				return new LineSegmentLinearX(line_x_a, line_x_b, p1.x, p2.x, false, Point2D(0, 0), false);
			}
			else
			{
				return new LineSegmentLinearY(line_y_a, line_y_b, p1.y, p2.y, false, Point2D(0, 0), false);
			}
		}
	}
	else
	{
		if (curve_x_loss < line_x_loss)
		{
			return new LineSegmentCurveY(curve_y_a, curve_y_b, curve_y_c, p1.y, p2.y, false, Point2D(0, 0), false);
		}
		else
		{
			if (abs(p1.x - p2.x) > abs(p1.y - p2.y))
			{
				return new LineSegmentLinearX(line_x_a, line_x_b, p1.x, p2.x, false, Point2D(0, 0), false);
			}
			else
			{
				return new LineSegmentLinearY(line_y_a, line_y_b, p1.y, p2.y, false, Point2D(0, 0), false);
			}
		}
	}
		


}

}