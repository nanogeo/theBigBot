#pragma once

#include "army_group.h"
#include "utility.h"

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"

#include "sc2api/sc2_unit_filters.h"


namespace sc2
{


class LineSegment
{
public:
	float a, b, c;
	float min;
	float max;

	virtual Point2D FindClosestPoint(Point2D);
};

class LineSegmentLinearX : public LineSegment
{
	// a(x+b)+c {min<=x<=max}
public:
	Point2D FindClosestPoint(Point2D) override;
};

class LineSegmentLinearY : public LineSegment
{
	// a(y+b)+c {min<=x<=max}
public:
	Point2D FindClosestPoint(Point2D) override;
};

class LineSegmentCurveX : public LineSegment
{
	// a(x+b)^2+c {min<=x<=max}
public:
	Point2D FindClosestPoint(Point2D) override;
};

class LineSegmentCurveY : public LineSegment
{
	// a(y+b)^2+c {min<=x<=max}
public:
	Point2D FindClosestPoint(Point2D) override;
};


class PathManager
{
public:
	std::vector<LineSegment> segments;

	Point2D FindClosestPoint(Point2D);
	Point2D GetPointFrom(Point2D, float);
};



}