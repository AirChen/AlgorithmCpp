#include "hull.h"
#include <set>
#include <stack>

namespace hull {
    int area2(const Point& p, const Point& q, const Point& s) {
        return p._x * q._y - p._y * q._x + q._x * s._y - q._y * s._x + s._x * p._y - s._y * p._x;
    }

    bool is_left(const Point& p, const Point& q, const Point& s) {
        return area2(p, q, s) > 0;
    }

    bool getHullNormal(const vector<Point*>& inPoly, vector<Point*>& outPoly) {
        std::set<Point*> selectedPoints;
        for (const auto& apt: inPoly) {
            for (const auto& bpt: inPoly) {
                // check left
                bool isValid = true;
                for (const auto& rpt: inPoly) {
                    if (is_left(*apt, *bpt, *rpt)) {
                        isValid = false;
                        break;
                    }
                }
                if (isValid) {
                    selectedPoints.insert(apt);
                    selectedPoints.insert(bpt);
                }
            }
        }

        for (const auto& pt: selectedPoints) {
            outPoly.emplace_back(pt);
        }
        return true;
    }

    bool sortPoints(const Point* p1, const Point* p2) {
        return (p1->_x - p2->_x) < 0.0000001 ? (p1->_y > p2->_y) : (p1->_x > p2->_y);
    }
    
    bool getHullOpt(const vector<Point*>& inPoly, vector<Point*>& outPoly) {
        vector<Point*> tmpPoly;
        std::sort(tmpPoly.begin(), tmpPoly.end(), sortPoints);
        
        std::stack<Point*> tCalcstack;
        size_t ptLen = tmpPoly.size();
        for (size_t i = 0; i < ptLen; i++) {
            if (i < 3) {
                tmpPoly[i]->marked();
                tCalcstack.push(tmpPoly[i]);
            } else {
                size_t preIdx = i - 1;
                while (preIdx > 0 && !is_left(*tmpPoly[preIdx - 1], *tmpPoly[i], *tmpPoly[preIdx])) {
                    auto& rpt = tCalcstack.top();
                    rpt->recover();
                    tCalcstack.pop();
                    preIdx -= 1;
                }
                tmpPoly[i]->marked();
                tCalcstack.push(tmpPoly[i]);
            }
        }

        std::reverse(tmpPoly.begin(), tmpPoly.end());

        std::stack<Point*> dCalcstack;
        for (size_t i = 0; i < ptLen; i++) {
            if (!tmpPoly[i]->isValid()) {
                continue;
            }
            if (i < 3) {
                tmpPoly[i]->marked();
                dCalcstack.push(tmpPoly[i]);
            } else {
                size_t preIdx = i - 1;
                while (preIdx > 0 && !is_left(*tmpPoly[preIdx - 1], *tmpPoly[i], *tmpPoly[preIdx])) {
                    auto& rpt = tCalcstack.top();
                    rpt->recover();
                    dCalcstack.pop();
                    preIdx -= 1;
                }
                tmpPoly[i]->marked();
                dCalcstack.push(tmpPoly[i]);
            }
        }

        while (!dCalcstack.empty()) {
            outPoly.emplace_back(dCalcstack.top());
            dCalcstack.pop();
        }

        while (!tCalcstack.empty()) {
            outPoly.emplace_back(tCalcstack.top());
            tCalcstack.pop();
        }

        return true;
    }
}
