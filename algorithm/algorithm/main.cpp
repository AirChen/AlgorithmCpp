//
//  main.cpp
//  algorithm
//
//  Created by tuRen on 2021/5/10.
//

#include <iostream>
#include "GeoHash.hpp"

using std::vector;
using std::cout;
using std::endl;
int main(int argc, const char * argv[]) {
    
    double lat = 40.390943;
    double lon = 75.9375;
    GeoHash hash = GeoHash::withCharacterPrecision(lat, lon, 12);
    
    
    cout << hash.toBase32() << endl;
    cout << hash.toBinaryString() << endl;
    cout << hash.toString() << endl;
    
    assert(0xcf6915015410500l == hash.ord());        
    
//    assertEquals(0xcf6915015410500l, hash.ord());
//    assertTrue(hash.ord() >= 0);
    
    return 0;
}
