#pragma once
#include <string>
#include <vector>
#include <map>

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_map_info.h"


namespace sc2 {


struct Vec2D
{
    Point2D start;
    Point2D end;
    Vec2D(Point2D s, Point2D e)
    {
        start = s;
        end = e;
    }
    bool operator==(const Vec2D& rhs)
    {
        if ((this->start == rhs.end && this->end == rhs.start) || (this->start == rhs.start && this->end == rhs.end))
            return true;
        return false;
    }
};

struct Polygon
{
    std::vector<Point2D> points;
    Polygon() {};
    Polygon(std::vector<Point2D> p)
    {
        points = p;
    }
};


struct Circle
{
    Point2D center;
    float radius = 0;
    Circle() {};
};

struct Triangle
{
    std::vector<Point2D> verticies;
    std::vector<Triangle*> connections;
    Point2D center;
	bool pathable;
    Triangle(std::vector<Point2D> p)
    {
        verticies = p;
		pathable = true;
        center = Point2D((verticies[0].x + verticies[1].x + verticies[2].x) / 3, (verticies[0].y + verticies[1].y + verticies[2].y) / 3);
    }
};

struct QTriangle
{
    float dist;
    Triangle* triangle;
    QTriangle(Triangle* triangle, float dist)
    {
        this->triangle = triangle;
        this->dist = dist;
    }
};

struct Portal
{
    Point2D left;
    Point2D right;
};

struct OverlapInfo
{
	Point2D pvector;
	int min;
	int max;
};

class NavMesh
{
public:
    std::vector<Triangle*> triangles;
	std::map<const Unit*, std::vector<Point2D>> obstacles;

    NavMesh() {};
    NavMesh(std::vector<Triangle*> triangles)
    {
        this->triangles = triangles;
    }

	void LoadOrBuildNavmesh(ImageData, std::string);
	std::vector<std::vector<bool>> SetUpMap(ImageData);
	//std::vector<std::vector<bool>> AddNeutralUnitsToMap(std::vector<std::vector<bool>>);
	std::vector<Vec2D> FindIsolines(std::vector<std::vector<bool>>);
	std::vector<Vec2D> GetIsolineConfiguration(bool, bool, bool, bool, Point2D, Point2D, Point2D, Point2D);
	std::vector<Polygon> MakePolygons(std::vector<Vec2D>);
	Polygon SimplifyPolygon(Polygon);
	std::vector<Point2D> GetAllVerticies(std::vector<Polygon>);
	std::vector<Polygon*> MakeTriangles(std::vector<Point2D>, ImageData);
	Circle ComputeCircumcircle(Polygon);
	Circle ComputeCircumcircle(Triangle*);
	Circle ComputeCircumcircle(Point2D, Point2D, Point2D);
	bool DoesShareVertex(Polygon, Polygon);
	bool DoesShareVertex(Triangle*, Triangle*);
	bool DoesShareSide(Triangle, Triangle);
	std::vector<Triangle*> ConvertToTriangles(std::vector<Polygon*>);
	std::vector<Triangle*> MarkOutsideTriangles(std::vector<Triangle*>, std::vector<std::vector<bool>>);
	void SaveNavMeshToFile(std::string);
	bool BuildNavMeshFromFile(std::string);

	void AddNewObstacle(const Unit*);
	Polygon CreateObstaclePolygon(const Unit*);
	std::vector<Triangle*> FindOverlappingTriangles(Polygon);
	std::vector<Point2D> FindIntersectionPoints(Polygon, std::vector<Triangle*>);
	Point2D FindLineSegmentIntersection(Point2D, Point2D, Point2D, Point2D);
	std::vector<Point2D> SaveVerticies(std::vector<Triangle*>);
	void RemoveTriangles(std::vector<Triangle*>);
	void ReAddVerticies(std::vector<Point2D>, std::vector<Point2D>);
	void AddVerticies(std::vector<Point2D>);

	void RemoveObstacle(const Unit*);
	void RemoveVertex(Point2D);
	std::vector<Point2D> FindConnectedVerticies(Point2D);
	void FlipEdge(Point2D, Point2D, Point2D, Point2D);
	Triangle* FindTriangle(Point2D, Point2D, Point2D);
	void RemoveTriangle(Triangle*);
	void AddTriangle(Triangle*);


    std::vector<Triangle*> ReconstructPath(std::map<Triangle*, Triangle*>, Triangle*);
    Triangle* FindClosestTriangle(Point2D);
    std::vector<Point2D> FindPath(Point2D, Point2D);
    std::vector<Triangle*> FindTrianglePath(Triangle*, Triangle*);
    static float TriangleSignPoint(Point2D, Point2D, Point2D);
    static bool PointInTriangle(Triangle*, Point2D);
    static Portal FindPortal(Triangle*, Triangle*);
    static std::vector<Point2D> FunnelPoints(std::vector<Portal>, Point2D, Point2D);
    static float Cross(Point2D, Point2D, Point2D);
	static float Cross(Point2D, Point2D);
    static int FindNextRightPoint(std::vector<Portal>, int);
    static int FindNextLeftPoint(std::vector<Portal>, int);
	static float Determinant(Point2D, Point2D, Point2D);
	static float RoundFloat(float);
};

}
