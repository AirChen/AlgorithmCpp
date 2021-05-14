//
//  main.cpp
//  tests
//
//  Created by tuRen on 2021/5/14.
//

#include <iostream>
#include "gtest/gtest.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
