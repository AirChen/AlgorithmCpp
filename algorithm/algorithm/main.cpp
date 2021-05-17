//
//  main.cpp
//  algorithm
//
//  Created by tuRen on 2021/5/10.
//

#include <iostream>

#include "VincentyGeodesy.hpp"
#include "WGS84Point.hpp"

using std::vector;
using std::cout;
using std::endl;
int main(int argc, const char * argv[]) {
    
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
