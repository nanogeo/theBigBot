#pragma once

namespace sc2
{

class LineSegment
{
protected:
	float a, b, c, min, max;
public:
	float GetMin() { return min; };
	float GetMax() { return max; };
	virtual Point2D EvaluateAt(float) = 0;
	virtual Point2D FindClosestPoint(Point2D) = 0;
};

class LineSegmentLinearX : public LineSegment
{
	// a(x+b)+c {min<=x<=max}
public:
	LineSegmentLinearX(float a, float b, float c, float min, float max)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		this->min = min;
		this->max = max;
	}
	Point2D EvaluateAt(float x) override { return Point2D(x, a * x + a * b + c); };
	Point2D FindClosestPoint(Point2D) override;
};

class LineSegmentLinearY : public LineSegment
{
	// a(y+b)+c {min<=x<=max}
public:
	LineSegmentLinearY(float a, float b, float c, float min, float max)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		this->min = min;
		this->max = max;
	}
	Point2D EvaluateAt(float y) override { return Point2D(a * y + a * b + c, y); };
	Point2D FindClosestPoint(Point2D) override;
};

class LineSegmentCurveX : public LineSegment
{
	// ax^2+bx+c {min<=x<=max}
public:
	LineSegmentCurveX(float a, float b, float c, float min, float max)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		this->min = min;
		this->max = max;
	}
	Point2D EvaluateAt(float x) override { return Point2D(x, a * pow(x, 2) + b * x + c); };
	Point2D FindClosestPoint(Point2D) override;
};

class LineSegmentCurveY : public LineSegment
{
	// ay^2+by+c {min<=x<=max}
public:
	LineSegmentCurveY(float a, float b, float c, float min, float max)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		this->min = min;
		this->max = max;
	}
	Point2D EvaluateAt(float y) override { return Point2D(a * pow(y, 2) + b * y + c, y); };
	Point2D FindClosestPoint(Point2D) override;
};


class PathManager
{
public:
	std::vector<LineSegment*> segments;
	PathManager() {};
	PathManager(std::vector<LineSegment*> segments)
	{
		this->segments = segments;
	}
	Point2D FindClosestPoint(Point2D);
	Point2D GetPointFrom(Point2D, float);
	std::vector<Point2D> GetPoints();
};



}