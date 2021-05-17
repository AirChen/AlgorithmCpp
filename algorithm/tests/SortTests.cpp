//
//  SortTests.cpp
//  tests
//
//  Created by tuRen on 2021/5/17.
//

#include "SortTests.hpp"
#include "gtest/gtest.h"
#include "Sort.hpp"

class SortTests : public ::testing::Test {
public:
    vector<int> g_vn;

    void SetUp() override {
        g_vn = {6, 5, 2, 7, 3, 9, 8, 4, 10, 1 }; // in per test cases.
    }
};


TEST_F(SortTests, test_insertion_sort) {
    InsertionSort *sort = new InsertionSort();
    sort->sort(g_vn);
    
    ASSERT_EQ(sort->isOrdered(g_vn), true);
    delete sort;
}

TEST_F(SortTests, test_selection_sort) {
    SelectionSort *sort = new SelectionSort();
    sort->sort(g_vn);

    ASSERT_EQ(sort->isOrdered(g_vn), true);
    delete sort;
}

TEST_F(SortTests, test_quick_sort) {
    QuickSort *sort = new QuickSort();
    sort->sort(g_vn);

    ASSERT_EQ(sort->isOrdered(g_vn), true);
    delete sort;
}

TEST_F(SortTests, test_quick_sort_3way) {
    QuickSort *sort = new QuickSort();
    sort->sort(g_vn, QuickSortType_3way);

    ASSERT_EQ(sort->isOrdered(g_vn), true);
    delete sort;
}

TEST_F(SortTests, test_shell_sort) {
    ShellSort *sort = new ShellSort();
    sort->sort(g_vn);

    ASSERT_EQ(sort->isOrdered(g_vn), true);
    delete sort;
}

TEST_F(SortTests, test_merge_sort) {
    MergeSort *sort = new MergeSort();
    sort->sort(g_vn);

    ASSERT_EQ(sort->isOrdered(g_vn), true);
    delete sort;
}

TEST_F(SortTests, test_heap_sort) {
    HeapSort *sort = new HeapSort();
    sort->sort(g_vn);
    
    g_vn.erase(g_vn.begin()); // remove place holder at index 0

    ASSERT_EQ(sort->isOrdered(g_vn), true);
    delete sort;
}
