#pragma once
#include <vector>
#include <map>

#include "sc2api/sc2_common.h"

#pragma warning(push)
#pragma warning(disable : 4244)

namespace sc2
{

	class LineSegment
	{
	protected:
		float min = 0;
		float max = 0;
		float start = 0;
		float end = 0;
		bool pos_direction = true;
	public:
		float GetMin() const { return min; };
		float GetMax() const { return max; };
		bool IsPositiveDirection() const { return pos_direction; };
		virtual Point2D EvaluateAt(float) const = 0;
		Point2D GetStartPoint() const { return EvaluateAt(start); };
		Point2D GetEndPoint() const { return EvaluateAt(end); };
		virtual Point2D FindClosestPoint(Point2D) const = 0;
		virtual std::vector<Point2D> FindCircleIntersection(Point2D, float) const = 0;
		virtual Point2D GetPointFrom(Point2D, float, bool, float&) const = 0;
		std::vector<Point2D> GetPoints() const
		{
			std::vector<Point2D> points;
			for (float i = min; i <= max; i++)
			{
				points.push_back(EvaluateAt(i));
			}
			return points;
		}
};

class LineSegmentLinearX : public LineSegment
{
	// ax+b {min<=x<=max}
private:
	float a, b;
public:
	LineSegmentLinearX(float a, float b, float min, float max)
	{
		this->a = a;
		this->b = b;
		this->start = min;
		this->end = max;
		this->min = std::min(min, max);
		this->max = std::max(min, max);
		pos_direction = min < max;
	}
	Point2D EvaluateAt(float x) const override { return Point2D(x, a * x + b); };
	Point2D FindClosestPoint(Point2D) const override;
	std::vector<Point2D> FindCircleIntersection(Point2D, float) const override;
	Point2D GetPointFrom(Point2D, float, bool, float&) const override;
};

class LineSegmentLinearY : public LineSegment
{
	// ay+b {min<=x<=max}
private:
	float a, b;
public:
	LineSegmentLinearY(float a, float b, float min, float max)
	{
		this->a = a;
		this->b = b;
		this->start = min;
		this->end = max;
		this->min = std::min(min, max);
		this->max = std::max(min, max);
		pos_direction = min < max;
	}
	Point2D EvaluateAt(float y) const override { return Point2D(a * y + b, y); };
	Point2D FindClosestPoint(Point2D) const override;
	std::vector<Point2D> FindCircleIntersection(Point2D, float) const override;
	Point2D GetPointFrom(Point2D, float, bool, float&) const override;
};

class LineSegmentCurveX : public LineSegment
{
	// ax^2+bx+c {min<=x<=max}
private:
	float a, b, c;
public:
	LineSegmentCurveX(float a, float b, float c, float min, float max)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		this->start = min;
		this->end = max;
		this->min = std::min(min, max);
		this->max = std::max(min, max);
		pos_direction = min < max;
	}
	Point2D EvaluateAt(float x) const override { return Point2D(x, a * pow(x, 2) + b * x + c); };
	Point2D FindClosestPoint(Point2D) const override;
	std::vector<Point2D> FindCircleIntersection(Point2D, float) const override;
	Point2D GetPointFrom(Point2D, float, bool, float&) const override;
};

class LineSegmentCurveY : public LineSegment
{
	// ay^2+by+c {min<=x<=max}
private:
	float a, b, c;
public:
	LineSegmentCurveY(float a, float b, float c, float min, float max)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		this->start = min;
		this->end = max;
		this->min = std::min(min, max);
		this->max = std::max(min, max);
		pos_direction = min < max;
	}
	Point2D EvaluateAt(float y) const override { return Point2D(a * pow(y, 2) + b * y + c, y); };
	Point2D FindClosestPoint(Point2D) const override;
	std::vector<Point2D> FindCircleIntersection(Point2D, float) const override;
	Point2D GetPointFrom(Point2D, float, bool, float&) const override;
};

// TODO change name cause this isnt a manager
class PiecewisePath
{
private:
	std::vector<LineSegment*> segments;
public:
	PiecewisePath() {};
	PiecewisePath(std::vector<LineSegment*> segments)
	{
		this->segments = segments;
	}
	PiecewisePath(std::vector<Point2D> points)
	{
		for (long unsigned int i = 0; i < points.size() - 1; i += 2)
		{
			// line between potins i , i+1
			if (abs(points[i].x - points[i + 1].x) > abs(points[i].y - points[i + 1].y))
			{
				float slope = (points[i].y - points[i + 1].y) / (points[i].x - points[i + 1].x);

				float line_x_a = slope;
				float line_x_b = points[i].y - (slope * points[i].x);

				segments.push_back(new LineSegmentLinearX(line_x_a, line_x_b, points[i].x, points[i + 1].x));
			}
			else
			{
				float slope = (points[i].x - points[i + 1].x) / (points[i].y - points[i + 1].y);

				float line_y_a = slope;
				float line_y_b = points[i].x - (slope * points[i].y);

				segments.push_back(new LineSegmentLinearY(line_y_a, line_y_b, points[i].y, points[i + 1].y));
			}
			if (i + 3 >= points.size())
				break;

			// curve between points i+1, i+2
			LineSegment* curve = FitLineSegment(points[i + 1], points[i + 2], points[i], points[i + 3]);
			segments.push_back(curve);
		}
	}
	Point2D FindClosestPoint(Point2D) const;
	int FindClosestSegmentIndex(Point2D) const;
	Point2D GetPointFrom(Point2D, float, bool) const;
	std::vector<Point2D> FindCircleIntersection(Point2D, float) const;
	std::vector<Point2D> GetPoints() const;
	Point2D GetStartPoint() const;
	Point2D GetEndPoint() const;
	Point2D GetFurthestForward(std::vector<Point2D>) const;
	Point2D GetFurthestBack(std::vector<Point2D>) const;

	LineSegment* FitLineSegment(Point2D, Point2D, Point2D, Point2D) const;
};

#pragma warning(pop)


}