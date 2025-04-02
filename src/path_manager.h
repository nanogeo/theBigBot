#pragma once

namespace sc2
{

	class LineSegment
	{
	protected:
		double a, b, c, min, max, start, end;
		bool pos_direction;
	public:
		double GetMin() { return min; };
		double GetMax() { return max; };
		bool IsPositiveDirection() { return pos_direction; };
		virtual Point2D EvaluateAt(double) const = 0;
		Point2D GetStartPoint() { return EvaluateAt(start); };
		Point2D GetEndPoint()  { return EvaluateAt(end); };
		virtual Point2D FindClosestPoint(Point2D) = 0;
		virtual std::vector<Point2D> FindCircleIntersection(Point2D, double) = 0;
		virtual Point2D GetPointFrom(Point2D, double, bool, double&) = 0;
		std::vector<Point2D> GetPoints() const
		{
			std::vector<Point2D> points;
			for (double i = min; i <= max; i++)
			{
				points.push_back(EvaluateAt(i));
			}
			return points;
		}
};

class LineSegmentLinearX : public LineSegment
{
	// ax+b {min<=x<=max}
public:
	LineSegmentLinearX(double a, double b, double min, double max, bool swap, Point2D center_point)
	{
		if (swap)
		{
			if (center_point.x != 0)
			{
				this->a = -1 * a;
				this->b = (center_point.x * a) + b;
				this->start = center_point.x - max;
				this->end = center_point.x - min;
				this->min = std::min(min, max);
				this->max = std::max(min, max);
			}
			else
			{
				// TODO for maps fliped on x axis
			}
		}
		else
		{
			this->a = a;
			this->b = b;
			this->start = min;
			this->end = max;
			this->min = std::min(min, max);
			this->max = std::max(min, max);
		}
		pos_direction = min < max;
	}
	Point2D EvaluateAt(double x) const override { return Point2D(x, a * x + b); };
	Point2D FindClosestPoint(Point2D) override;
	std::vector<Point2D> FindCircleIntersection(Point2D, double) override;
	Point2D GetPointFrom(Point2D, double, bool, double&) override;
};

class LineSegmentLinearY : public LineSegment
{
	// ay+b {min<=x<=max}
public:
	LineSegmentLinearY(double a, double b, double min, double max, bool swap, Point2D center_point)
	{
		if (swap)
		{
			Point2D intercept = Point2D(0, -1 * b / a);
			Point2D flipped_intercept = Point2D(2 * center_point.x, (2 * center_point.y) - intercept.y);
			this->a = a;
			this->b = flipped_intercept.x - (a * flipped_intercept.y);
			this->start = (2 * center_point.y) - max;
			this->end = (2 * center_point.y - min);
			this->min = std::min(min, max);
			this->max = std::max(min, max);
		}
		else
		{
			this->a = a;
			this->b = b;
			this->start = min;
			this->end = max;
			this->min = std::min(min, max);
			this->max = std::max(min, max);
		}
		pos_direction = min < max;
	}
	Point2D EvaluateAt(double y) const override { return Point2D(a * y + b, y); };
	Point2D FindClosestPoint(Point2D) override;
	std::vector<Point2D> FindCircleIntersection(Point2D, double) override;
	Point2D GetPointFrom(Point2D, double, bool, double&) override;
};

class LineSegmentCurveX : public LineSegment
{
	// ax^2+bx+c {min<=x<=max}
public:
	LineSegmentCurveX(double a, double b, double c, double min, double max, bool swap, Point2D center_point)
	{
		if (swap)
		{
			if (center_point.x != 0)
			{
				this->a = a;
				this->b = -2 * a * (center_point.x + (b / (2 * a)));
				this->c = a * pow(-1 * b / (2 * a), 2) - (pow(b, 2) / (2 * a)) + c + (a * pow(center_point.x + (b / (2 * a)), 2));
				this->start = center_point.x - max;
				this->end = center_point.x - min;
				this->min = std::min(min, max);
				this->max = std::max(min, max);
			}
			else
			{
				// TODO for maps fliped on x axis
			}
		}
		else
		{
			this->a = a;
			this->b = b;
			this->c = c;
			this->start = min;
			this->end = max;
			this->min = std::min(min, max);
			this->max = std::max(min, max);
		}
		pos_direction = min < max;
	}
	Point2D EvaluateAt(double x) const override { return Point2D(x, a * pow(x, 2) + b * x + c); };
	Point2D FindClosestPoint(Point2D) override;
	std::vector<Point2D> FindCircleIntersection(Point2D, double) override;
	Point2D GetPointFrom(Point2D, double, bool, double&) override;
};

class LineSegmentCurveY : public LineSegment
{
	// ay^2+by+c {min<=x<=max}
public:
	LineSegmentCurveY(double a, double b, double c, double min, double max, bool swap, Point2D center_point)
	{
		if (swap)
		{
			float h = -1 * b / (2 * a);
			float k = c - (a * pow(h, 2));
			Point2D flipped_vertex = Point2D((2 * center_point.x) - k, (2 * center_point.y) - h);
			this->a = -1 * a;
			this->b = 2 * a * flipped_vertex.y;
			this->c = flipped_vertex.x - (a * pow(flipped_vertex.y, 2));
			this->start = (2 * center_point.y) - max;
			this->end = (2 * center_point.y - min);
			this->min = std::min(min, max);
			this->max = std::max(min, max);
		}
		else
		{
			this->a = a;
			this->b = b;
			this->c = c;
			this->start = min;
			this->end = max;
			this->min = std::min(min, max);
			this->max = std::max(min, max);
		}
		pos_direction = min < max;
	}
	Point2D EvaluateAt(double y) const override { return Point2D(a * pow(y, 2) + b * y + c, y); };
	Point2D FindClosestPoint(Point2D) override;
	std::vector<Point2D> FindCircleIntersection(Point2D, double) override;
	Point2D GetPointFrom(Point2D, double, bool, double&) override;
};

// TODO change name cause this isnt a manager
class PathManager
{
public:
	std::vector<LineSegment*> segments;
	PathManager() {};
	PathManager(std::vector<LineSegment*> segments)
	{
		this->segments = segments;
	}
	PathManager(std::vector<Point2D> points)
	{
		for (long unsigned int i = 0; i < points.size() - 1; i += 2)
		{
			// line between potins i , i+1
			if (abs(points[i].x - points[i + 1].x) > abs(points[i].y - points[i + 1].y))
			{
				double slope = (points[i].y - points[i + 1].y) / (points[i].x - points[i + 1].x);

				double line_x_a = slope;
				double line_x_b = points[i].y - (slope * points[i].x);

				segments.push_back(new LineSegmentLinearX(line_x_a, line_x_b, points[i].x, points[i + 1].x, false, Point2D(0, 0)));
			}
			else
			{
				double slope = (points[i].x - points[i + 1].x) / (points[i].y - points[i + 1].y);

				double line_y_a = slope;
				double line_y_b = points[i].x - (slope * points[i].y);

				segments.push_back(new LineSegmentLinearY(line_y_a, line_y_b, points[i].y, points[i + 1].y, false, Point2D(0, 0)));
			}
			if (i + 3 >= points.size())
				break;

			// curve between points i+1, i+2
			LineSegment* curve = FitLineSegment(points[i + 1], points[i + 2], points[i], points[i + 3]);
			segments.push_back(curve);
		}
	}
	Point2D FindClosestPoint(Point2D);
	int FindClosestSegmentIndex(Point2D);
	Point2D GetPointFrom(Point2D, double, bool);
	std::vector<Point2D> FindCircleIntersection(Point2D, double);
	std::vector<Point2D> GetPoints();
	Point2D GetStartPoint();
	Point2D GetEndPoint();
	Point2D GetStart();
	Point2D GetEnd();
	Point2D GetFurthestForward(std::vector<Point2D>);
	Point2D GetFurthestBack(std::vector<Point2D>);

	LineSegment* FitLineSegment(Point2D, Point2D, Point2D, Point2D);
};



}