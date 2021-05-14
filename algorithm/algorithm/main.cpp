//
//  main.cpp
//  algorithm
//
//  Created by tuRen on 2021/5/10.
//

#include <iostream>
#include "Sort.hpp"
#include "PrimaryQueue.hpp"

#include "VincentyGeodesy.hpp"
#include "WGS84Point.hpp"

using std::vector;
using std::cout;
using std::endl;
int main(int argc, const char * argv[]) {
//    vector<int> vn = {6, 5, 2, 7, 3, 9, 8, 4, 10, 1 };
    
//    QuickSort sort;
//    sort.sort(vn, QuickSortType_normal);
    
//    HeapSort sort;
//    sort.sort(vn);
//
//    cout << "result: " << endl;
//    for (auto num : vn) {
//        cout << num << endl;
//    }
    
//    PrimaryQueue<int>* pq = new PrimaryQueue<int>(3, PrimaryQueueType_MIN);
//    for (auto v : vn) {
//        pq->insert(v);
//    }
//
//    do {
//        cout << pq->top() << endl;
//    } while (pq->delTop());
//
//    delete pq;
    
    
    
    // VincentyGeodesy test
    WGS84Point startPoint(40, 40);
    VincentyGeodesy geodesy;
    int distanceInMeters = 10000;
    WGS84Point result = geodesy.moveInDirection(startPoint, 120, distanceInMeters);
    
    assert(approximatelyEqual(40.10134882, result.getLongitude()));
    assert(approximatelyEqual(39.9549245, result.getLatitude()));
    assert(approximatelyEqual(distanceInMeters, geodesy.distanceInMeters(startPoint, result)));

    WGS84Point p1(1, 1);
    int tenThousandKilometers = 10000000;
    WGS84Point p2 = geodesy.moveInDirection(p1, 270, tenThousandKilometers);
    
    cout << p2.toString() << endl;
    assert(approximatelyEqual(tenThousandKilometers, geodesy.distanceInMeters(p1, p2)));
        
    
    // hash test
    WGS84Point a(47.2342, 15.7465465);
    WGS84Point b{a};
    WGS84Point c(-47.234, b.getLongitude());
    WGS84Point d(-32.9687253, 12.42334242);
    
    assert(a.hashCode() == a.hashCode());
    assert(a.hashCode() == b.hashCode());
    assert(a.hashCode() != c.hashCode());
    assert(d.hashCode() != c.hashCode());
    
    return 0;
}
