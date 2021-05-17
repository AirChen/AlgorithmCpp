//
//  BoundingBox.hpp
//  algorithm
//
//  Created by tuRen on 2021/5/14.
//

#ifndef BoundingBox_hpp
#define BoundingBox_hpp

#include <stdio.h>
#include "WGS84Point.hpp"

class BoundingBox {
    double southLatitude;
    double northLatitude;
    double westLongitude;
    double eastLongitude;
    bool intersects180Meridian;
    
    bool containsLatitude(double latitude) {
        return latitude >= southLatitude && latitude <= northLatitude;
    }

    bool containsLongitude(double longitude) {
        if (intersects180Meridian) {
            return longitude <= eastLongitude || longitude >= westLongitude;
        } else {
            return longitude >= westLongitude && longitude <= eastLongitude;
        }
    }
    
public:
    BoundingBox() {};
    
    BoundingBox(double southLatitude, double northLatitude, double westLongitude, double eastLongitude) {
        assert(southLatitude <= northLatitude);

        
        if (abs(southLatitude) > 90 || abs(northLatitude) > 90 || abs(westLongitude) > 180 || abs(eastLongitude) > 180) {
            printf("error: The supplied coordinates are out of range. \n");
        }

        this->northLatitude = northLatitude;
        this->westLongitude = westLongitude;

        this->southLatitude = southLatitude;
        this->eastLongitude = eastLongitude;

        intersects180Meridian = eastLongitude < westLongitude;
    }
    
    BoundingBox(WGS84Point southWestCorner, WGS84Point northEastCorner):
    BoundingBox(southWestCorner.getLatitude(), northEastCorner.getLatitude(), southWestCorner.getLongitude(), northEastCorner.getLongitude()) {};
    
    BoundingBox(const BoundingBox& that):
    BoundingBox(that.southLatitude, that.northLatitude, that.westLongitude, that.eastLongitude) {};
    
    WGS84Point getNorthWestCorner() {
        return {northLatitude, westLongitude};
    }
    
    WGS84Point getNorthEastCorner() {
        return {northLatitude, eastLongitude};
    }
    
    WGS84Point getSouthEastCorner() {
        return {southLatitude, eastLongitude};
    }
    
    WGS84Point getSouthWestCorner() {
        return {southLatitude, westLongitude};
    }
    
    double getLatitudeSize() {
        return northLatitude - southLatitude;
    }
    
    double getLongitudeSize() {
        if (eastLongitude == 180.0 && westLongitude == -180.0)
            return 360.0;
        double size = (eastLongitude - westLongitude) > 360.0 ? abs(eastLongitude - westLongitude - 360.0) : (eastLongitude - westLongitude);
        if (size < 0)
            size += 360.0;
        return size;
    }
    
    double getEastLongitude() {
        return eastLongitude;
    }

    double getWestLongitude() {
        return westLongitude;
    }

    double getNorthLatitude() {
        return northLatitude;
    }

    double getSouthLatitude() {
        return southLatitude;
    }

    bool getIntersects180Meridian() {
        return intersects180Meridian;
    }
    
    bool contains(WGS84Point point) {
        return containsLatitude(point.getLatitude()) && containsLongitude(point.getLongitude());
    }

    bool intersects(BoundingBox other) {
        // Check latitude first cause it's the same for all cases
        if (other.southLatitude > northLatitude || other.northLatitude < southLatitude) {
            return false;
        } else {
            if (!intersects180Meridian && !other.intersects180Meridian) {
                return !(other.eastLongitude < westLongitude || other.westLongitude > eastLongitude);
            } else if (intersects180Meridian && !other.intersects180Meridian) {
                return !(eastLongitude < other.westLongitude && westLongitude > other.eastLongitude);
            } else if (!intersects180Meridian && other.intersects180Meridian) {
                return !(westLongitude > other.eastLongitude && eastLongitude < other.westLongitude);
            } else
                return true;
        }
    }
    
    WGS84Point getCenter() {
        double centerLatitude = (southLatitude + northLatitude) / 2;
        double centerLongitude = (westLongitude + eastLongitude) / 2;

        // This can happen if the bBox crosses the 180-Meridian
        if (centerLongitude > 180)
            centerLongitude -= 360;

        return {centerLatitude, centerLongitude};
    }
    
    /**
     * Expands this bounding box to include the provided bounding box. The expansion is done in the direction with the minimal distance. If both distances are the same it'll expand
     * in east direction. It will not cross poles, but it will cross the 180-Meridian, if thats the shortest distance.<br>
     * If a precise specification of the northEast and southWest points is needed, please create a new bounding box where you can specify the points separately.
     */
    void expandToInclude(BoundingBox& other) {
        // Expand Latitude
        if (other.southLatitude < southLatitude) {
            southLatitude = other.southLatitude;
        }
        if (other.northLatitude > northLatitude) {
            northLatitude = other.northLatitude;
        }

        // Expand Longitude
        // At first check whether the two boxes contain each other or not
        bool thisContainsOther = containsLongitude(other.eastLongitude) && containsLongitude(other.westLongitude);
        bool otherContainsThis = other.containsLongitude(eastLongitude) && other.containsLongitude(westLongitude);

        // The new box needs to span the whole globe
        if (thisContainsOther && otherContainsThis) {
            eastLongitude = 180.0;
            westLongitude = -180.0;
            intersects180Meridian = false;
            return;
        }
        // Already done in this case
        if (thisContainsOther)
            return;
        // Expand to match the bigger box
        if (otherContainsThis) {
            eastLongitude = other.eastLongitude;
            westLongitude = other.westLongitude;
            intersects180Meridian = eastLongitude < westLongitude;
            return;
        }

        // If this is not the case compute the distance between the endpoints in east direction
        double distanceEastToOtherEast = (other.eastLongitude - eastLongitude) > 360.0 ? abs(other.eastLongitude - eastLongitude - 360.0) : (other.eastLongitude - eastLongitude);
        double distanceOtherWestToWest = (westLongitude - other.westLongitude) > 360.0 ? abs(westLongitude - other.westLongitude - 360.0) : (westLongitude - other.westLongitude);

        // Fix for lower java versions, since the remainder-operator (%) changed in one version, idk which one
        if (distanceEastToOtherEast < 0)
            distanceEastToOtherEast += 360;
        if (distanceOtherWestToWest < 0)
            distanceOtherWestToWest += 360;

        // The minimal distance needs to be extended
        if (distanceEastToOtherEast <= distanceOtherWestToWest) {
            eastLongitude = other.eastLongitude;
        } else {
            westLongitude = other.westLongitude;
        }

        intersects180Meridian = eastLongitude < westLongitude;
    }
    
    string toString() {
        return getNorthWestCorner().toString() + " -> " + getSouthEastCorner().toString();
    }
    
    bool operator==(const BoundingBox& obj) const {
        return southLatitude == obj.southLatitude && westLongitude == obj.westLongitude && northLatitude == obj.northLatitude && eastLongitude == obj.eastLongitude;
    }
    
    int hashCode() {
        int result = 17;
        result = 37 * result + hashCode(southLatitude);
        result = 37 * result + hashCode(northLatitude);
        result = 37 * result + hashCode(westLongitude);
        result = 37 * result + hashCode(eastLongitude);
        return result;
    }

    int hashCode(double x) {
        long f = doubleToRawBits(x);
        return (int) (f ^ (f >> 32));
    }
};

#endif /* BoundingBox_hpp */
