//
//  GeoHashTests.cpp
//  tests
//
//  Created by tuRen on 2021/5/18.
//

#include "GeoHashTests.hpp"
#include "gtest/gtest.h"
#include "GeoHash.hpp"

#include <string>
#include <vector>

using std::string;

class GeoHashTests: public ::testing::Test {
    
public:
    void assertEncodingWithCharacterPrecision(WGS84Point point, int numberOfCharacters, string stringValue) {
        GeoHash hash = GeoHash::withCharacterPrecision(point.getLatitude(), point.getLongitude(), numberOfCharacters);
        ASSERT_EQ(stringValue, hash.toBase32());
    }
    
    void checkMovingInCircle(double latitude, double longitude) {
        GeoHash start;
        GeoHash end;
        start = GeoHash::withCharacterPrecision(latitude, longitude, 12);
        end = start.getEasternNeighbour();
        end = end.getSouthernNeighbour();
        end = end.getWesternNeighbour();
        end = end.getNorthernNeighbour();
        
        ASSERT_EQ(start.toBase32(), end.toBase32());
        ASSERT_EQ(start.getBoundingBox().toString(), end.getBoundingBox().toString());
    }
};

TEST_F(GeoHashTests, test_characterPrecision) {
    assertEncodingWithCharacterPrecision({20, 31}, 12, "sew1c2vs2q5r");
    assertEncodingWithCharacterPrecision({-20, 31}, 12, "ksqn1rje83g2");
    assertEncodingWithCharacterPrecision({-20.783236276, 31.9867127312312}, 12, "ksq9zbs0b7vw");

    WGS84Point point(-76.5110040642321, 39.0247389581054);
    string fullStringValue = "hf7u8p8gn747";
    for (int characters = 12; characters > 1; characters--) {
        assertEncodingWithCharacterPrecision(point, characters, fullStringValue.substr(0, characters));
    }

    assertEncodingWithCharacterPrecision({39.0247389581054, -76.5110040642321}, 12, "dqcw4bnrs6s7");

    string geoHashString = GeoHash::geoHashStringWithCharacterPrecision(point.getLatitude(), point.getLongitude(), 12);
    ASSERT_EQ(fullStringValue, geoHashString);
}

TEST_F(GeoHashTests, test_getLatitudeBits) {
    GeoHash hash = GeoHash::withBitPrecision(30, 30, 16);
    long *latitudeBits = (long*)malloc(sizeof(long) * 2);
    hash.getRightAlignedLatitudeBits(latitudeBits);
    ASSERT_EQ(0xaal, latitudeBits[0]);
    ASSERT_EQ(8, latitudeBits[1]);
    
    free(latitudeBits);
}

TEST_F(GeoHashTests, test_getLongitudeBits) {
    GeoHash hash = GeoHash::withBitPrecision(30, 30, 16);
    long *longitudeBits = (long*)malloc(sizeof(long) * 2);
    hash.getRightAlignedLongitudeBits(longitudeBits);
    ASSERT_EQ(0x95l, longitudeBits[0]);
    ASSERT_EQ(8, longitudeBits[1]);
    
    free(longitudeBits);
}

TEST_F(GeoHashTests, test_neighbourLocationCode) {
    GeoHash hash;
    
    // set up corner case
    hash.bits = 0xc400000000000000l;
    hash.significantBits = 7;

    long *lonBits = (long*)malloc(sizeof(long)*2);
    hash.getRightAlignedLongitudeBits(lonBits);
    ASSERT_EQ(0x8, lonBits[0]);
    ASSERT_EQ(4, lonBits[1]);
    free(lonBits);

    long *latBits = (long*)malloc(sizeof(long)*2);
    hash.getRightAlignedLatitudeBits(latBits);
    ASSERT_EQ(0x5, latBits[0]);
    ASSERT_EQ(3, latBits[1]);
    free(latBits);

    GeoHash north = hash.getNorthernNeighbour();
    ASSERT_EQ(0xd000000000000000l, north.bits);
    ASSERT_EQ(7, north.significantBits);

    GeoHash south = hash.getSouthernNeighbour();
    ASSERT_EQ(0xc000000000000000l, south.bits);
    ASSERT_EQ(7, south.significantBits);

    GeoHash east = hash.getEasternNeighbour();
    ASSERT_EQ(0xc600000000000000l, east.bits);

    // NOTE: this is actually a corner case!
    GeoHash west = hash.getWesternNeighbour();
    ASSERT_EQ(0x6e00000000000000l, west.bits);

    // NOTE: and now, for the most extreme corner case in 7-bit geohash-land
    hash.bits = 0xfe00000000000000l;

    GeoHash east_other = hash.getEasternNeighbour();
    ASSERT_EQ(0x5400000000000000l, east_other.bits);
}

TEST_F(GeoHashTests, test_equalsAndHashCode) {
    GeoHash hash1 = GeoHash::withBitPrecision(30, 30, 24);
    GeoHash hash2 = GeoHash::withBitPrecision(30, 30, 24);
    GeoHash hash3 = GeoHash::withBitPrecision(30, 30, 10);
    
    ASSERT_TRUE(hash1.equals(hash2) && hash2.equals(hash1));
    ASSERT_FALSE(hash1.equals(hash3) && hash3.equals(hash1));
    
    ASSERT_EQ(hash1.hashCode(), hash2.hashCode());
    ASSERT_FALSE(hash1.hashCode() == hash3.hashCode());
}

using std::vector;
TEST_F(GeoHashTests, test_adjacentHashes) {
    vector<GeoHash> adjacent = GeoHash::fromGeohashString("dqcw4").getAdjacent();
    ASSERT_EQ(8, adjacent.size());
}

TEST_F(GeoHashTests, testMovingInCircle) {
    // moving around hashes in a circle should be possible
    checkMovingInCircle(34.2, -45.123);
    // this should also work at the "back" of the earth
    checkMovingInCircle(45, 180);
    checkMovingInCircle(90, 180);
    checkMovingInCircle(0, -180);
}
