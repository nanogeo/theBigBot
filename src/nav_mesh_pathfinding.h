#pragma once
#include <string>

#include "sc2api/sc2_interfaces.h"


namespace sc2 {


struct Triangle
{
    std::vector<Point2D> verticies;
    std::vector<Triangle*> connections;
    Point2D center;
    Triangle() {};
    Triangle(std::vector<Point2D> p)
    {
        verticies = p;
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

class NavMesh
{
public:
    std::vector<Triangle*> triangles;

    NavMesh() {};
    NavMesh(std::vector<Triangle*> triangles)
    {
        this->triangles = triangles;
    }

    std::vector<Triangle*> ReconstructPath(std::map<Triangle*, Triangle*>, Triangle*);
    Triangle* FindClosestTriangle(Point2D);
    std::vector<Point2D> FindPath(Point2D, Point2D);
    std::vector<Triangle*> FindTrianglePath(Triangle*, Triangle*);
    static float TriangleSignPoint(Point2D, Point2D, Point2D);
    static bool PointInTriangle(Triangle*, Point2D);
    static Portal FindPortal(Triangle*, Triangle*);
    static std::vector<Point2D> FunnelPoints(std::vector<Portal>, Point2D, Point2D);
    static float Cross(Point2D, Point2D, Point2D);
    static int FindNextRightPoint(std::vector<Portal>, int);
    static int FindNextLeftPoint(std::vector<Portal>, int);
	void SaveNavMeshToFile(std::string);
	void BuildNavMeshFromFile(std::string);
};

}
