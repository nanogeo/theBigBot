#pragma once
#include "path.h"
#include "pathing_manager.h"

#include <vector>
#include <map>

#include "sc2api/sc2_common.h"

namespace sc2
{

class PointPath : public Path
{
private:
	std::vector<Point2D> points;
public:
	PointPath() {};
	PointPath(std::vector<Point2D> points)
	{
		this->points = points;
	}
	Point2D FindClosestPoint(Point2D) const override;
	Point2D GetPointFrom(Point2D, float, bool) const override;
	std::vector<Point2D> FindCircleIntersection(Point2D, float) const override;
	std::vector<Point2D> GetPoints() const override;
	Point2D GetStartPoint() const override;
	Point2D GetEndPoint() const override;
	Point2D GetFurthestForward(std::vector<Point2D>) const override;
	Point2D GetFurthestBack(std::vector<Point2D>) const override;

};


}