#pragma once
#include <vector>

#include "sc2api/sc2_common.h"

namespace sc2
{

class Path
{
public:
	virtual Point2D FindClosestPoint(Point2D) const = 0;
	virtual Point2D GetPointFrom(Point2D, float, bool) const = 0;
	virtual std::vector<Point2D> FindCircleIntersection(Point2D, float) const = 0;
	virtual std::vector<Point2D> GetPoints() const = 0;
	virtual Point2D GetStartPoint() const = 0;
	virtual Point2D GetEndPoint() const = 0;
	virtual Point2D GetFurthestForward(std::vector<Point2D>) const = 0;
	virtual Point2D GetFurthestBack(std::vector<Point2D>) const = 0;
};



}