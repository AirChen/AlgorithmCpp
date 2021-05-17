//
//  PrimaryQueueTests.cpp
//  tests
//
//  Created by tuRen on 2021/5/17.
//

#include "PrimaryQueueTests.hpp"
#include "gtest/gtest.h"
#include "PrimaryQueue.hpp"

using std::vector;

class PrimaryQueueTest: public ::testing::Test {
public:
    vector<int> _vn = {6, 5, 2, 7, 3, 9, 8, 4, 10, 1 };
};

TEST_F(PrimaryQueueTest, test_max_queue) {
    PrimaryQueue<int>* pq = new PrimaryQueue<int>(3);
    for (auto v : _vn) {
        pq->insert(v);
    }

    int last = 10010;
    do {
        ASSERT_TRUE(last >= pq->top());
        last = pq->top();
    } while (pq->delTop());

    delete pq;
}

TEST_F(PrimaryQueueTest, test_min_queue) {
    PrimaryQueue<int>* pq = new PrimaryQueue<int>(3, PrimaryQueueType_MIN);
    for (auto v : _vn) {
        pq->insert(v);
    }

    int last = -10010;
    do {
        ASSERT_TRUE(last <= pq->top());
        last = pq->top();
    } while (pq->delTop());

    delete pq;
}
