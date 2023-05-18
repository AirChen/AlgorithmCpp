//
//  WGS84Point.hpp
//  algorithm
//
//  Created by tuRen on 2021/5/14.
//

#ifndef WGS84Point_hpp
#define WGS84Point_hpp

#include <stdio.h>
#include <string>
#include <cmath>

inline uint64_t doubleToRawBits(double x) {
    uint64_t bits;
    memcpy(&bits, &x, sizeof bits);
    return bits;
}

inline bool approximatelyEqual(double lhs, double rhs) {
    return fabs(lhs - rhs) < 0.0001;
}

using std::string;
using std::to_string;

class WGS84Point {
    double latitude;
    double longitude;
    
public:
    WGS84Point() {};
    
    WGS84Point(double lat, double lon): latitude(lat), longitude(lon)
    {
        assert(abs(lat) <= 90 || abs(lon) <= 180);
    }
    
    WGS84Point(const WGS84Point& other): WGS84Point(other.latitude, other.longitude) {}
    
    ~WGS84Point() {};
    
    double getLatitude() { return latitude; }
    double getLongitude() { return longitude; }
    
    string toString()
    {
        return "(" + to_string(latitude) + "," + to_string(longitude) + ")";
    }

    bool operator==(const WGS84Point& obj) const {
        return latitude == obj.latitude && longitude == obj.longitude;
    }
    
    WGS84Point &operator=(const WGS84Point& obj) {
        latitude = obj.latitude;
        longitude = obj.longitude;
        return *this;
    }
    
    int hashCode()
    {
        int result = 42;
        long latBits = doubleToRawBits(latitude);
        long lonBits = doubleToRawBits(longitude);
        result = 31 * result + (int) (latBits ^ (latBits >> 32));
        result = 31 * result + (int) (lonBits ^ (lonBits >> 32));
        return result;
    }
};

#endif /* WGS84Point_hpp */
