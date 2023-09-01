
#include "TossBot.h"
#include "path_manager.h"
#include "locations.h"

constexpr float PI = 3.14159265358979323;

namespace sc2
{

Point2D LineSegmentLinearX::FindClosestPoint(Point2D point)
{
	float perp_slope = - 1 / a;
	float perp_coef = point.y - point.x * perp_slope;
	float coef = a * b + c;

	Point2D closest_pos = Point2D((perp_coef - coef) / (a - perp_slope), a * ((perp_coef - coef) / (a - perp_slope)) + coef);

	if (closest_pos.x < min)
		return Point2D(min, a * (min + b) + c);
	if (closest_pos.x > max)
		return Point2D(max, a * (max + b) + c);
	return closest_pos;
}

Point2D LineSegmentLinearX::FindClosestPoint(Point2D point)
{
	float perp_slope = -1 / a;
	float perp_coef = point.x - point.y * perp_slope;
	float coef = a * b + c;

	Point2D closest_pos = Point2D(a * ((perp_coef - coef) / (a - perp_slope)) + coef, (perp_coef - coef) / (a - perp_slope));

	if (closest_pos.y < min)
		return Point2D(a * (min + b) + c, min);
	if (closest_pos.y > max)
		return Point2D(a * (max + b) + c, max);
	return closest_pos;
}

Point2D LineSegmentCurveX::FindClosestPoint(Point2D point)
{
	// derivative of distance
	float a1 = 4 * pow(a, 2);
	float b1 = 6 * a * b;
	float c1 = 2 + 2 * pow(b, 2) + 4 * a * c - 4 * a * point.y;
	float d1 = 2 * b * c - 2 * point.x - 2 * b * point.y;

	// cubic formula
	float p = (c1 / a1) - (pow(b1, 2) / (3 * pow(a1, 2)));
	float q = ((2 * pow(b1, 3)) / (27 * pow(a1, 3))) - ((b1 * c1) / (3 * pow(a1, 2))) + (d1 / a1);

	float num_roots = pow(q / 2, 2) + pow(p / 3, 3);

	if (num_roots > 0)
	{
		// only 1 solutions
		float x_value = std::cbrt(-1 * (q / 2) - std::sqrt(num_roots)) + std::cbrt(-1 * (q / 2) + std::sqrt(num_roots)) - (b1 / (3 * a1));
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
		float r = -1 * q / 2;
		float i = -1 * sqrt(abs(num_roots));
		float dist = sqrt(pow(r, 2) + pow(i, 2));
		float angle = atan2(i, r);

		float s1 = 2 * std::cbrt(dist) * cos(angle / 3) - (b1 / (3 * a1));
		float s2 = 2 * std::cbrt(dist) * cos((angle + 2 * PI) / 3) - (b1 / (3 * a1));
		float s3 = 2 * std::cbrt(dist) * cos((angle + 4 * PI) / 3) - (b1 / (3 * a1));

		Point2D p1 = Point2D(s1, a * pow(s1, 2) + b * s1 + c);
		Point2D p2 = Point2D(s2, a * pow(s2, 2) + b * s2 + c);
		Point2D p3 = Point2D(s3, a * pow(s3, 2) + b * s3 + c);

		float dist1 = Distance2D(point, p1);
		float dist2 = Distance2D(point, p2);
		float dist3 = Distance2D(point, p3);

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

Point2D LineSegmentCurveX::FindClosestPoint(Point2D point)
{
	// derivative of distance
	float a1 = 4 * pow(a, 2);
	float b1 = 6 * a * b;
	float c1 = 2 + 2 * pow(b, 2) + 4 * a * c - 4 * a * point.x;
	float d1 = 2 * b * c - 2 * point.y - 2 * b * point.x;

	// cubic formula
	float p = (c1 / a1) - (pow(b1, 2) / (3 * pow(a1, 2)));
	float q = ((2 * pow(b1, 3)) / (27 * pow(a1, 3))) - ((b1 * c1) / (3 * pow(a1, 2))) + (d1 / a1);

	float num_roots = pow(q / 2, 2) + pow(p / 3, 3);

	if (num_roots > 0)
	{
		// only 1 solutions
		float y_value = std::cbrt(-1 * (q / 2) - std::sqrt(num_roots)) + std::cbrt(-1 * (q / 2) + std::sqrt(num_roots)) - (b1 / (3 * a1));
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
		float r = -1 * q / 2;
		float i = -1 * sqrt(abs(num_roots));
		float dist = sqrt(pow(r, 2) + pow(i, 2));
		float angle = atan2(i, r);

		float s1 = 2 * std::cbrt(dist) * cos(angle / 3) - (b1 / (3 * a1));
		float s2 = 2 * std::cbrt(dist) * cos((angle + 2 * PI) / 3) - (b1 / (3 * a1));
		float s3 = 2 * std::cbrt(dist) * cos((angle + 4 * PI) / 3) - (b1 / (3 * a1));

		Point2D p1 = Point2D(a * pow(s1, 2) + b * s1 + c, s1);
		Point2D p2 = Point2D(a * pow(s2, 2) + b * s2 + c, s2);
		Point2D p3 = Point2D(a * pow(s3, 2) + b * s3 + c, s3);

		float dist1 = Distance2D(point, p1);
		float dist2 = Distance2D(point, p2);
		float dist3 = Distance2D(point, p3);

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

}