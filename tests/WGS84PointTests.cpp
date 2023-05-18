//
//  WGS84PointTests.cpp
//  tests
//
//  Created by tuRen on 2021/5/17.
//

#include "WGS84PointTests.hpp"
#include "gtest/gtest.h"
#include "VincentyGeodesy.hpp"
#include "WGS84Point.hpp"

using std::cout;
using std::endl;

class WGS84PointTests: public ::testing::Test {
};

TEST_F(WGS84PointTests, test_VincentyGeodesy) {
    WGS84Point startPoint(40, 40);
    VincentyGeodesy geodesy;
    int distanceInMeters = 10000;
    WGS84Point result = geodesy.moveInDirection(startPoint, 120, distanceInMeters);
    
    ASSERT_FLOAT_EQ(40.10134882, result.getLongitude());
    ASSERT_FLOAT_EQ(39.9549245, result.getLatitude());
    ASSERT_FLOAT_EQ(distanceInMeters, geodesy.distanceInMeters(startPoint, result));

    WGS84Point p1(1, 1);
    int tenThousandKilometers = 10000000;
    WGS84Point p2 = geodesy.moveInDirection(p1, 270, tenThousandKilometers);
    
    cout << p2.toString() << endl;
    ASSERT_FLOAT_EQ(tenThousandKilometers, geodesy.distanceInMeters(p1, p2));
}

TEST_F(WGS84PointTests, test_hashCode) {
    WGS84Point a(47.2342, 15.7465465);
    WGS84Point b{a};
    WGS84Point c(-47.234, b.getLongitude());
    WGS84Point d(-32.9687253, 12.42334242);
    
    ASSERT_EQ(a.hashCode(), a.hashCode());
    ASSERT_EQ(a.hashCode(), b.hashCode());
    ASSERT_NE(a.hashCode(), c.hashCode());
    ASSERT_NE(d.hashCode(), c.hashCode());
}
