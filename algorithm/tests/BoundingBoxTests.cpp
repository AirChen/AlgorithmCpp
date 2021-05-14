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

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    void SetUp() override {
     // Code here will be called immediately after the constructor (right
     // before each test).
    }

    void TearDown() override {
     // Code here will be called immediately after each test (right
     // before the destructor).
    }
};


TEST_F(BoundingBoxTests, test_hashCode)
{
    ASSERT_EQ(a->hashCode(), b->hashCode());
    ASSERT_FALSE(a->hashCode() == c->hashCode());
}


