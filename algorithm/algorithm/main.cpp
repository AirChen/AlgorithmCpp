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
    double lat = -20.783236276;
    double lon = 31.9867127312312;
    GeoHash hash = GeoHash::withCharacterPrecision(lat, lon, 12);
    
    cout << hash.toBase32() << endl;
    cout << hash.toBinaryString() << endl;
    cout << hash.toString() << endl;
    
    return 0;
}
