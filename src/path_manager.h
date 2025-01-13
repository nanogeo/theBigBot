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
		virtual Point2D EvaluateAt(double) = 0;
		Point2D GetStart() { return EvaluateAt(start); };
		Point2D GetEnd() { return EvaluateAt(end); };
		virtual Point2D FindClosestPoint(Point2D) = 0;
		virtual std::vector<Point2D> FindCircleIntersection(Point2D, double) = 0;
		virtual Point2D GetPointFrom(Point2D, double, bool, double&) = 0;
		std::vector<Point2D> GetPoints()
		{
			std::vector<Point2D> points;
			for (int i = min; i <= max; i++)
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
	LineSegmentLinearX(double a, double b, double min, double max, bool swap, Point2D center_point, bool rotated)
	{
		if (swap)
		{
			if (rotated)
			{
				Point2D intercept = Point2D(-1 * b / a, 0);
				Point2D flipped_intercept = Point2D((2 * center_point.x) - intercept.x, 2 * center_point.y);
				this->a = a;
				this->b = flipped_intercept.y - (a * flipped_intercept.x);
				this->start = (2 * center_point.x) - max;
				this->end = (2 * center_point.x - min);
				this->min = std::min(min, max);
				this->max = std::max(min, max);
			}
			else
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
	Point2D EvaluateAt(double x) override { return Point2D(x, a * x + b); };
	Point2D FindClosestPoint(Point2D) override;
	std::vector<Point2D> FindCircleIntersection(Point2D, double) override;
	Point2D GetPointFrom(Point2D, double, bool, double&) override;
};

class LineSegmentLinearY : public LineSegment
{
	// ay+b {min<=x<=max}
public:
	LineSegmentLinearY(double a, double b, double min, double max, bool swap, Point2D center_point, bool rotated)
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
	Point2D EvaluateAt(double y) override { return Point2D(a * y + b, y); };
	Point2D FindClosestPoint(Point2D) override;
	std::vector<Point2D> FindCircleIntersection(Point2D, double) override;
	Point2D GetPointFrom(Point2D, double, bool, double&) override;
};

class LineSegmentCurveX : public LineSegment
{
	// ax^2+bx+c {min<=x<=max}
public:
	LineSegmentCurveX(double a, double b, double c, double min, double max, bool swap, Point2D center_point, bool rotated)
	{
		if (swap)
		{
			if (rotated)
			{
				float h = -1 * b / (2 * a);
				float k = c - (a * pow(h, 2));
				Point2D flipped_vertex = Point2D((2 * center_point.x) - h, (2 * center_point.y) - k);
				this->a = -1 * a;
				this->b = 2 * a * flipped_vertex.x;
				this->c = flipped_vertex.y - (a * pow(flipped_vertex.x, 2));
				this->start = (2 * center_point.x) - max;
				this->end = (2 * center_point.x - min);
				this->min = std::min(min, max);
				this->max = std::max(min, max);
			}
			else
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
	Point2D EvaluateAt(double x) override { return Point2D(x, a * pow(x, 2) + b * x + c); };
	Point2D FindClosestPoint(Point2D) override;
	std::vector<Point2D> FindCircleIntersection(Point2D, double) override;
	Point2D GetPointFrom(Point2D, double, bool, double&) override;
};

class LineSegmentCurveY : public LineSegment
{
	// ay^2+by+c {min<=x<=max}
public:
	LineSegmentCurveY(double a, double b, double c, double min, double max, bool swap, Point2D center_point, bool rotated)
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
	Point2D EvaluateAt(double y) override { return Point2D(a * pow(y, 2) + b * y + c, y); };
	Point2D FindClosestPoint(Point2D) override;
	std::vector<Point2D> FindCircleIntersection(Point2D, double) override;
	Point2D GetPointFrom(Point2D, double, bool, double&) override;
};


class PathManager
{
public:
	std::vector<LineSegment*> segments;
	PathManager() {};
	PathManager(std::vector<LineSegment*> segments, bool x_based, bool pos_direction)
	{
		this->segments = segments;
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

	LineSegment* FitLineSegment(Point2D, Point2D, Point2D, Point2D);
};



}