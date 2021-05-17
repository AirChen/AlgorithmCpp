//
//  BoundingBoxTests.cpp
//  tests
//
//  Created by tuRen on 2021/5/14.
//

#include "BoundingBoxTests.hpp"
#include "gtest/gtest.h"
#include "BoundingBox.hpp"

class BoundingBoxTests : public ::testing::Test {
public:
    const double DELTA = 1e-12;
    BoundingBox *a;
    BoundingBox *b;
    BoundingBox *c;
    BoundingBox *d;
    BoundingBox *e;
    
protected:

    BoundingBoxTests() {
        a = new BoundingBox((WGS84Point){21, 20}, (WGS84Point){30, 31});
        b = new BoundingBox(*a);
        c = new BoundingBox({-45, -170}, {45, 170});
        d = new BoundingBox({-45, 170}, {-45, -170});
        e = new BoundingBox(*d);
    }

    ~BoundingBoxTests() override {
        free(a);
        free(b);
        free(c);
        free(d);
        free(e);
    }
    
    static void assertDoesNotIntersect(BoundingBox bbox, BoundingBox boundingBox) {
        ASSERT_FALSE(bbox.intersects(boundingBox));
        ASSERT_FALSE(boundingBox.intersects(bbox));
    }

    static void assertIntersects(BoundingBox bbox, BoundingBox boundingBox) {
        ASSERT_TRUE(bbox.intersects(boundingBox));
        ASSERT_TRUE(boundingBox.intersects(bbox));
    }
};


TEST_F(BoundingBoxTests, test_hashCode)
{
    ASSERT_EQ(a->hashCode(), b->hashCode());
    ASSERT_FALSE(a->hashCode() == c->hashCode());
}

TEST_F(BoundingBoxTests, test_contains) {
    BoundingBox bbox(45, 46, 120, 121);
    ASSERT_TRUE(bbox.contains({45.5, 120.5}));
    ASSERT_FALSE(bbox.contains({90, 90}));
    
    bbox = {45, 46, 170, -170};
    ASSERT_TRUE(bbox.contains({45.5, 175}));
    ASSERT_TRUE(bbox.contains({45.5, -175}));
    ASSERT_FALSE(bbox.contains({45.5, -165}));
    ASSERT_FALSE(bbox.contains({45.5, 165}));
}

TEST_F(BoundingBoxTests, test_size) {
    BoundingBox bbox(45, 90, 0, 30);
    ASSERT_FLOAT_EQ(bbox.getLatitudeSize(), 45);
    ASSERT_FLOAT_EQ(bbox.getLongitudeSize(), 30);
    bbox = {-45, 45, -22.5, 30};
    ASSERT_FLOAT_EQ(bbox.getLatitudeSize(), 90);
    ASSERT_FLOAT_EQ(bbox.getLongitudeSize(), 52.5);
    bbox = {-46.1, -44, -128, -127.2};
    ASSERT_FLOAT_EQ(bbox.getLatitudeSize(), 2.1);
    ASSERT_FLOAT_EQ(bbox.getLongitudeSize(), 0.8);

    // Testing bounding box over 180-Meridian
    bbox = {45, 90, 170, -170};
    ASSERT_FLOAT_EQ(bbox.getLatitudeSize(), 45);
    ASSERT_FLOAT_EQ(bbox.getLongitudeSize(), 20);
}

TEST_F(BoundingBoxTests, test_intersects) {
    BoundingBox bbox(-10, 10, 40, 41);
    assertIntersects(bbox, {-15, 5, 40.5, 43});
    assertDoesNotIntersect(bbox, {-15, 5, 42, 43});

    // Testing bounding box over 180-Meridian
    bbox = {45, 90, 170, -170};
    assertIntersects(bbox, {50, 55, 175, 176});
    assertIntersects(bbox, {50, 55, 160, 176});
    assertIntersects(bbox, {50, 55, -175, -176});
    assertIntersects(bbox, {50, 55, -160, -176});
    assertIntersects(bbox, {50, 55, 175, -175});
    assertIntersects(bbox, {50, 55, -175, 175});

    assertDoesNotIntersect(bbox, {-15, 5, 42, 43});
    assertDoesNotIntersect(bbox, {-15, 5, 175, 176});
    assertDoesNotIntersect(bbox, {-15, 5, 175, -175});
    assertDoesNotIntersect(bbox, {50, 55, 160, 169});
    assertDoesNotIntersect(bbox, {50, 55, -169, -160});
}
