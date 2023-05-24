#ifndef hull_hpp
#define hull_hpp

#include <vector>

using std::vector;

namespace hull {
    struct Point {
        double _x;
        double _y;
        double _valid;
        Point (double x, double y): _x(x), _y(y) {
            _valid = true;
        }

        inline void marked() {
            _valid = false;
        }

        inline bool isValid() {
            return _valid;
        }

        inline void recover() {
            _valid = true;
        }
    };

    // 蛮力算法
    bool getHullNormal(const vector<Point*>& inPoly, vector<Point*>& outPoly);

    bool getHullOpt(const vector<Point*>& inPoly, vector<Point*>& outPoly);
}

#endif
