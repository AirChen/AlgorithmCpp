//
//  VincentyGeodesy.cpp
//  algorithm
//
//  Created by tuRen on 2021/5/14.
//

#include "VincentyGeodesy.hpp"

WGS84Point VincentyGeodesy::moveInDirection(WGS84Point& point, double bearingInDegrees, double distanceInMeters)
{
    assert((bearingInDegrees >= 0 && bearingInDegrees <= 360));

    double a = 6378137, b = 6356752.3142, f = 1 / 298.257223563; // WGS-84
    // ellipsiod
    double alpha1 = bearingInDegrees * degToRad;
    double sinAlpha1 = sin(alpha1), cosAlpha1 = cos(alpha1);

    double tanU1 = (1 - f) * tan(point.getLatitude() * degToRad);
    double cosU1 = 1 / sqrt((1 + tanU1 * tanU1)), sinU1 = tanU1 * cosU1;
    double sigma1 = atan2(tanU1, cosAlpha1);
    double sinAlpha = cosU1 * sinAlpha1;
    double cosSqAlpha = 1 - sinAlpha * sinAlpha;
    double uSq = cosSqAlpha * (a * a - b * b) / (b * b);
    double A = 1 + uSq / 16384 * (4096 + uSq * (-768 + uSq * (320 - 175 * uSq)));
    double B = uSq / 1024 * (256 + uSq * (-128 + uSq * (74 - 47 * uSq)));

    double sinSigma = 0, cosSigma = 0, cos2SigmaM = 0;
    double sigma = distanceInMeters / (b * A), sigmaP = 2 * M_PI;
    while (abs(sigma - sigmaP) > 1e-12) {
        cos2SigmaM = cos(2 * sigma1 + sigma);
        sinSigma = sin(sigma);
        cosSigma = cos(sigma);
        double deltaSigma = B
                * sinSigma
                * (cos2SigmaM + B
                        / 4
                        * (cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM) - B / 6 * cos2SigmaM
                                * (-3 + 4 * sinSigma * sinSigma) * (-3 + 4 * cos2SigmaM * cos2SigmaM)));
        sigmaP = sigma;
        sigma = distanceInMeters / (b * A) + deltaSigma;
    }

    double tmp = sinU1 * sinSigma - cosU1 * cosSigma * cosAlpha1;
    double lat2 = atan2(sinU1 * cosSigma + cosU1 * sinSigma * cosAlpha1, (1 - f) * sqrt(sinAlpha * sinAlpha + tmp * tmp));
    double lambda = atan2(sinSigma * sinAlpha1, cosU1 * cosSigma - sinU1 * sinSigma * cosAlpha1);
    double C = f / 16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));
    double L = lambda - (1 - C) * f * sinAlpha
            * (sigma + C * sinSigma * (cos2SigmaM + C * cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM)));

    double newLat = lat2 / degToRad;
    double newLon = point.getLongitude() + L / degToRad;

    newLon = (newLon > 180.0 ? newLon - 360 : newLon);
    newLon = (newLon < -180.0 ? 360.0 + newLon : newLon);

    return {newLat, newLon};
}

double VincentyGeodesy::distanceInMeters(WGS84Point& foo, WGS84Point& bar)
{
    double a = 6378137, b = 6356752.3142, f = 1 / 298.257223563; // WGS-84
    // ellipsiod
    double L = (bar.getLongitude() - foo.getLongitude()) * degToRad;
    double U1 = atan((1 - f) * tan(foo.getLatitude() * degToRad));
    double U2 = atan((1 - f) * tan(bar.getLatitude() * degToRad));
    double sinU1 = sin(U1), cosU1 = cos(U1);
    double sinU2 = sin(U2), cosU2 = cos(U2);

    double cosSqAlpha, sinSigma, cos2SigmaM, cosSigma, sigma;

    double lambda = L, lambdaP, iterLimit = 20;
    do {
        double sinLambda = sin(lambda), cosLambda = cos(lambda);
        sinSigma = sqrt((cosU2 * sinLambda) * (cosU2 * sinLambda)
                + (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda) * (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda));
        if (sinSigma == 0) {
            return 0; // co-incident points
        }
        cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosLambda;
        sigma = atan2(sinSigma, cosSigma);
        double sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;
        cosSqAlpha = 1 - sinAlpha * sinAlpha;
        cos2SigmaM = cosSigma - 2 * sinU1 * sinU2 / cosSqAlpha;
        if (cos2SigmaM == NAN) {
            cos2SigmaM = 0; // equatorial line: cosSqAlpha=0
        }
        double C = f / 16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));
        lambdaP = lambda;
        lambda = L + (1 - C) * f * sinAlpha
                * (sigma + C * sinSigma * (cos2SigmaM + C * cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM)));
    } while (abs(lambda - lambdaP) > EPSILON && --iterLimit > 0);

    if (iterLimit == 0) {
        return NAN;
    }
    double uSquared = cosSqAlpha * (a * a - b * b) / (b * b);
    double A = 1 + uSquared / 16384 * (4096 + uSquared * (-768 + uSquared * (320 - 175 * uSquared)));
    double B = uSquared / 1024 * (256 + uSquared * (-128 + uSquared * (74 - 47 * uSquared)));
    double deltaSigma = B
            * sinSigma
            * (cos2SigmaM + B
                    / 4
                    * (cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM) - B / 6 * cos2SigmaM
                            * (-3 + 4 * sinSigma * sinSigma) * (-3 + 4 * cos2SigmaM * cos2SigmaM)));
    double s = b * A * (sigma - deltaSigma);
    
    return s;
}
