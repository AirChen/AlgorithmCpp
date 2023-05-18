//
//  VincentyGeodesy.hpp
//  algorithm
//
//  Created by tuRen on 2021/5/14.
//

#ifndef VincentyGeodesy_hpp
#define VincentyGeodesy_hpp

#include "WGS84Point.hpp"
#include <stdio.h>

class VincentyGeodesy {
    /* double equatorRadius = 6378137, poleRadius = 6356752.3142, f = 1 / 298.257223563; */
    /* double equatorRadiusSquared = equatorRadius * equatorRadius, poleRadiusSquared = poleRadius * poleRadius; */
public:
    const double degToRad = 0.0174532925199433;
    const double EPSILON = 1e-12;
    
    WGS84Point moveInDirection(WGS84Point& point, double bearingInDegrees, double distanceInMeters);
    double distanceInMeters(WGS84Point& foo, WGS84Point& bar);
};

#endif /* VincentyGeodesy_hpp */
