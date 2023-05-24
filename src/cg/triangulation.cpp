#include "triangulation.h"

namespace mapimage {

    bool almost_equal_t(const wsl::Point& v, const wsl::Point& w) {
        return fabs(v.x() - w.x()) < std::numeric_limits<double>::epsilon() 
        && fabs(v.y() - w.y()) < std::numeric_limits<double>::epsilon();
    }
    
    bool almost_equal_e(const EdgeType& v, const EdgeType& w) {
        return almost_equal_t(v.v, w.v) && almost_equal_t(v.w, w.w) 
        || almost_equal_t(v.v, w.w) && almost_equal_t(v.w, w.v);
    }

    double norm2(const wsl::Point& p)
    {
        return p.x() * p.x() + p.y() * p.y();
    }

    double dist2(const wsl::Point& v, const wsl::Point& w)
    {
        const double dx = v.x() - w.x();
        const double dy = v.y() - w.y();
        return dx * dx + dy * dy;
    }

    bool TriangulationProcessor::triangulate_polygon(wsl::Polygon& in_polygon, std::vector<wsl::Polygon>& out_polygons) {
        // Store the vertices locally
	    wsl::Polygon _vertices = in_polygon;

        // Determinate the super triangle
        double minX = in_polygon[0].x();
        double minY = in_polygon[0].y();
        double maxX = minX;
        double maxY = minY;

        for(size_t i = 0; i < in_polygon.size(); ++i)
        {
            if (in_polygon[i].x() < minX) minX = in_polygon[i].x();
            if (in_polygon[i].y() < minY) minY = in_polygon[i].y();
            if (in_polygon[i].x() > maxX) maxX = in_polygon[i].x();
            if (in_polygon[i].y() > maxY) maxY = in_polygon[i].y();
        }

        const double dx = maxX - minX;
        const double dy = maxY - minY;
        const double deltaMax = std::max(dx, dy);
        const double midx = (minX + maxX) / 2;
        const double midy = (minY + maxY) / 2;

        wsl::Point p1(midx - 20 * deltaMax, midy - deltaMax);
        wsl::Point p2(midx, midy + 20 * deltaMax);
        wsl::Point p3(midx + 20 * deltaMax, midy - deltaMax);

        // Create a list of triangles, and add the supertriangle in it
        std::vector<TriangleType> triangles;
        triangles.push_back(TriangleType(p1, p2, p3));

        // for (auto p : in_polygon)
        for (size_t idx = 0; idx < in_polygon.size(); idx++)
        {
            wsl::Point p = in_polygon[idx];
            std::vector<EdgeType> polygon;

            for(auto &t : triangles)
            {
                if(t.circumCircleContains(p))
                {
                    t.isBad = true;
                    polygon.push_back(EdgeType(t.a, t.b));
                    polygon.push_back(EdgeType(t.b, t.c));
                    polygon.push_back(EdgeType(t.c, t.a));
                }
            }

            triangles.erase(std::remove_if(begin(triangles), end(triangles), [](TriangleType &t){
                return t.isBad;
            }), end(triangles));

            for(auto e1 = begin(polygon); e1 != end(polygon); ++e1)
            {
                for(auto e2 = e1 + 1; e2 != end(polygon); ++e2)
                {
                    if(almost_equal_e(*e1, *e2))
                    {
                        e1->isBad = true;
                        e2->isBad = true;
                    }
                }
            }

            polygon.erase(std::remove_if(begin(polygon), end(polygon), [](EdgeType &e){
                return e.isBad;
            }), end(polygon));

            for(const auto e : polygon) {
                triangles.push_back(TriangleType(e.v, e.w, p));
            }
        }

        triangles.erase(std::remove_if(begin(triangles), end(triangles), [p1, p2, p3](TriangleType &t){
            return t.containsVertex(p1) || t.containsVertex(p2) || t.containsVertex(p3);
        }), end(triangles));

        for(auto t : triangles)
        {
            wsl::Polygon triangle;
            triangle.push_back(t.a);
            triangle.push_back(t.b);
            triangle.push_back(t.c);
            out_polygons.emplace_back(triangle);
        }

        return true;
    }
}