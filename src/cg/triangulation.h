#pragma once

#include <math.h>
#include <limits>
#include <type_traits>
#include <algorithm>
#include <vector>
#include "spatial-base.h"
#include "type_factory.h"

namespace mapimage {

    bool almost_equal_t(const wsl::Point& v, const wsl::Point& w);
    double norm2(const wsl::Point& p);
    double dist2(const wsl::Point& v, const wsl::Point& w);

    struct TriangleType
    {
        wsl::Point a;
        wsl::Point b;
        wsl::Point c;
        bool isBad;

        TriangleType(wsl::Point a1, wsl::Point b1, wsl::Point c1) {
            a = a1;
            b = b1;
            c = c1;
            isBad = false;
        };

        bool circumCircleContains(const wsl::Point& p) {
            const double ab = norm2(a);
            const double cd = norm2(b);
            const double ef = norm2(c);

            const double ax = a.x();
            const double ay = a.y();
            const double bx = b.x();
            const double by = b.y();
            const double cx = c.x();
            const double cy = c.y();

            const double circum_x = (ab * (cy - by) + cd * (ay - cy) + ef * (by - ay)) / (ax * (cy - by) + bx * (ay - cy) + cx * (by - ay));
            const double circum_y = (ab * (cx - bx) + cd * (ax - cx) + ef * (bx - ax)) / (ay * (cx - bx) + by * (ax - cx) + cy * (bx - ax));

            wsl::Point circum(circum_x / 2, circum_y / 2);
            const double circum_radius = dist2(a, circum);
            const double dist = dist2(p, circum);
            return dist <= circum_radius;
        };

        bool containsVertex(const wsl::Point& p) {
            return almost_equal_t(a, p) || almost_equal_t(b, p) || almost_equal_t(c, p);
        };
    };

    struct EdgeType
    {
        wsl::Point v;
        wsl::Point w;
        bool isBad;

        EdgeType(wsl::Point a, wsl::Point b) {
            v = a;
            w = b;
            isBad = false;
        }
    };

    class TriangulationProcessor {
    public:
        bool triangulate_polygon(wsl::Polygon& in_polygon, std::vector<wsl::Polygon>& out_polygons);
    };
}