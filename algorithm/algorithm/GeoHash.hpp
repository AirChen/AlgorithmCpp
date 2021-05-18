//
//  GeoHash.hpp
//  algorithm
//
//  Created by tuRen on 2021/5/17.
//

#ifndef GeoHash_hpp
#define GeoHash_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include "WGS84Point.hpp"
#include "BoundingBox.hpp"

using std::fill;
using std::vector;
using std::string;

static int count = 0;
static char *base32;
static int *decodeArray;

class GeoHash {
public:
    static const int MAX_BIT_PRECISION = 64;
    static const int MAX_CHARACTER_PRECISION = 12;
    
    static const long serialVersionUID = -8553214249630252175L;
    constexpr static const int BITS[] = { 16, 8, 4, 2, 1 };
    static const int BASE32_BITS = 5;
    static const long FIRST_BIT_FLAGGED = 0x8000000000000000l;

    long bits = 0;
    int significantBits = 0;
    
private:
    WGS84Point point;
    BoundingBox boundingBox;
    
public:
    GeoHash() {
        if (count == 0) {
            initDecodeArray();
        }
        count++;
    }
    
    ~GeoHash() {
        count--;
        if (count == 0) {
            delete [] base32;
            delete [] decodeArray;
        }
    }
    
    static GeoHash withCharacterPrecision(double latitude, double longitude, int numberOfCharacters) {
        if (numberOfCharacters > MAX_CHARACTER_PRECISION) {
            printf("error: A geohash can only be %d character long.\n", MAX_CHARACTER_PRECISION);
        }
        int desiredPrecision = (numberOfCharacters * 5 <= 60) ? numberOfCharacters * 5 : 60;
        
        GeoHash ans(latitude, longitude, desiredPrecision);
        return ans;
    }
    
    static string geoHashStringWithCharacterPrecision(double latitude, double longitude, int numberOfCharacters) {
        GeoHash hash = withCharacterPrecision(latitude, longitude, numberOfCharacters);
        return hash.toBase32();
    }
    
    static GeoHash withBitPrecision(double latitude, double longitude, int numberOfBits) {
        if (numberOfBits > MAX_BIT_PRECISION) {
            printf("error: A Geohash can only be %d bits long! \n", MAX_BIT_PRECISION);
        }
        if (abs(latitude) > 90.0 || abs(longitude) > 180.0) {
            printf("error: Can't have lat/lon values out of (-90,90)/(-180/180)\n");
        }
        return GeoHash(latitude, longitude, numberOfBits);
    }
    
    static GeoHash fromLongValue(long hashVal, int significantBits) {
        double latitudeRange[] = { -90.0, 90.0 };
        double longitudeRange[] = { -180.0, 180.0 };

        bool isEvenBit = true;
        GeoHash hash;

        string binaryString = to_string(hashVal);
        while (binaryString.length() < MAX_BIT_PRECISION) {
            binaryString = "0" + binaryString;
        }
        for (int j = 0; j < significantBits; j++) {
            if (isEvenBit) {
                divideRangeDecode(hash, longitudeRange, binaryString[j] != '0');
            } else {
                divideRangeDecode(hash, latitudeRange, binaryString[j] != '0');
            }
            isEvenBit = !isEvenBit;
        }

        double latitude = (latitudeRange[0] + latitudeRange[1]) / 2;
        double longitude = (longitudeRange[0] + longitudeRange[1]) / 2;

        hash.point = {latitude, longitude};
        setBoundingBox(hash, latitudeRange, longitudeRange);
        hash.bits <<= (MAX_BIT_PRECISION - hash.significantBits);
        return hash;
    }
    
    static GeoHash fromGeohashString(string geohash) {
        double latitudeRange[] = { -90.0, 90.0 };
        double longitudeRange[] = { -180.0, 180.0 };

        bool isEvenBit = true;
        GeoHash hash;

        int decodeArraySize = (int)sizeof(decodeArray)/sizeof(decodeArray[0]);
        for (int i = 0; i < geohash.length(); i++) {
            char c = geohash[i];
            int cd = 0;
            if (c >= decodeArraySize || (cd = decodeArray[c]) < 0) {
                printf("error: Invalid character character '%c' in geohash '%s'! \n", c, geohash.c_str());
            }
            for (int j = 0; j < BASE32_BITS; j++) {
                int mask = BITS[j];
                if (isEvenBit) {
                    divideRangeDecode(hash, longitudeRange, (cd & mask) != 0);
                } else {
                    divideRangeDecode(hash, latitudeRange, (cd & mask) != 0);
                }
                isEvenBit = !isEvenBit;
            }
        }

        double latitude = (latitudeRange[0] + latitudeRange[1]) / 2;
        double longitude = (longitudeRange[0] + longitudeRange[1]) / 2;

        hash.point = {latitude, longitude};
        setBoundingBox(hash, latitudeRange, longitudeRange);
        hash.bits <<= (MAX_BIT_PRECISION - hash.significantBits);
        return hash;
    }
    
    int getCharacterPrecision() {
        if (significantBits % 5 != 0) {
            printf("error: precision of GeoHash is not divisble by. \n");
        }
        return significantBits / 5;
    }
    
    static long stepsBetween(GeoHash& one, GeoHash& two) {
        if (one.significantBits != two.significantBits) {
            printf("error: It is only valid to compare the number of steps between two hashes if they have the same number of significant bits \n");
        }
        return two.ord() - one.ord();
    }
    
    vector<GeoHash> getAdjacent() {
        GeoHash northern = getNorthernNeighbour();
        GeoHash eastern = getEasternNeighbour();
        GeoHash southern = getSouthernNeighbour();
        GeoHash western = getWesternNeighbour();
        
        return {northern, northern.getEasternNeighbour(), eastern, southern.getEasternNeighbour(),
            southern,
            southern.getWesternNeighbour(), western, northern.getWesternNeighbour()
        };
    }
    
    string toBase32() {
        if (significantBits % 5 != 0) {
            printf("error: Cannot convert a geohash to base32 if the precision is not a multiple of 5. \n");
        }

        string buf;

        long bitsCopy = bits;
        int partialChunks = (int)ceil(((double)significantBits / 5));

        int leftMove = 59;
        for (int i = 0; i < partialChunks; i++) {
            int pointer = (int)((bitsCopy >> leftMove) & 31);
            char chr = base32[pointer];
            
            buf += chr;
            leftMove -= 5;
        }
        return buf;
    }
    
    string toBinaryString() {
        string bui;
        long bitsCopy = bits;
        for (int i = 0; i < significantBits; i++) {
            if ((bitsCopy & FIRST_BIT_FLAGGED) == FIRST_BIT_FLAGGED) {
                bui += '1';
            } else {
                bui += '0';
            }
            bitsCopy <<= 1;
        }
        return bui;
    }
    
    string toString() {
        string ans = to_string(bits);
        if (significantBits % 5 == 0) {
            ans += " -> ";
            ans += boundingBox.toString();
            ans += " -> ";
            ans += toBase32();
        } else {
            ans += " -> ";
            ans += boundingBox.toString();
            ans += ", bits:";
            ans += to_string(significantBits);
        }
        return ans;
    }
    
    static GeoHash fromOrd(long ord, int significantBits) {
        int insignificantBits = MAX_BIT_PRECISION - significantBits;
        return fromLongValue(ord << insignificantBits, significantBits);
    }
    
    GeoHash next(int step) {
        return fromOrd(ord() + step, significantBits);
    }

    GeoHash next() {
        return next(1);
    }

    GeoHash prev() {
        return next(-1);
    }

    long ord() {
        int insignificantBits = MAX_BIT_PRECISION - significantBits;
        return bits >> insignificantBits;
    }
    
    BoundingBox getBoundingBox() {
        return boundingBox;
    }
    
private:
    GeoHash(double latitude, double longitude, int desiredPrecision) {
        if (count == 0) {
            initDecodeArray();
        }
        count++;
        
        point = {latitude, longitude};
        desiredPrecision = fmin(desiredPrecision, MAX_BIT_PRECISION);

        bool isEvenBit = true;
        double latitudeRange[] = { -90, 90 };
        double longitudeRange[] = { -180, 180 };

        while (significantBits < desiredPrecision) {
            if (isEvenBit) {
                divideRangeEncode(longitude, longitudeRange);
            } else {
                divideRangeEncode(latitude, latitudeRange);
            }
            isEvenBit = !isEvenBit;
        }

        setBoundingBox(*this, latitudeRange, longitudeRange);
        bits <<= (MAX_BIT_PRECISION - desiredPrecision);
    }
    
    void initDecodeArray() {
        const int base32Size = 32;
        const int size = 'z' + 1;
                
        base32 = new char[base32Size];
        char tmp[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'j', 'k', 'm', 'n', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
        for (int i = 0; i < base32Size; i++) {
            base32[i] = tmp[i];
        }
        
        decodeArray = new int[size];
        fill(decodeArray, decodeArray + size, -1);
        for (int i = 0; i < base32Size; i++) {
            decodeArray[base32[i]] = i;
        }
    }
    
    static void setBoundingBox(GeoHash& hash, double * const latitudeRange, double * const longitudeRange) {
        hash.boundingBox = {latitudeRange[0], latitudeRange[1], longitudeRange[0], longitudeRange[1]};
    }
    
    void divideRangeEncode(double value, double* range) {
        double mid = (range[0] + range[1]) / 2;
        if (value >= mid) {
            addOnBitToEnd();
            range[0] = mid;
        } else {
            addOffBitToEnd();
            range[1] = mid;
        }
    }

    static void divideRangeDecode(GeoHash& hash, double* range, bool b) {
        double mid = (range[0] + range[1]) / 2;
        if (b) {
            hash.addOnBitToEnd();
            range[0] = mid;
        } else {
            hash.addOffBitToEnd();
            range[1] = mid;
        }
    }
    
    long mask() {
        if (significantBits == 0) {
            return 0;
        } else {
            long value = FIRST_BIT_FLAGGED;
            value >>= (significantBits - 1);
            return value;
        }
    }
    
    long maskLastNBits(long value, long n) {
        long mask = 0xffffffffffffffffl;
        mask >>= (MAX_BIT_PRECISION - n);
        return value & mask;
    }
    
    GeoHash recombineLatLonBitsToHash(long* latBits, long* lonBits) {
        GeoHash hash;
        bool isEvenBit = false;
        latBits[0] <<= (MAX_BIT_PRECISION - latBits[1]);
        lonBits[0] <<= (MAX_BIT_PRECISION - lonBits[1]);
        double latitudeRange[] = { -90.0, 90.0 };
        double longitudeRange[] = { -180.0, 180.0 };

        for (int i = 0; i < latBits[1] + lonBits[1]; i++) {
            if (isEvenBit) {
                divideRangeDecode(hash, latitudeRange, (latBits[0] & FIRST_BIT_FLAGGED) == FIRST_BIT_FLAGGED);
                latBits[0] <<= 1;
            } else {
                divideRangeDecode(hash, longitudeRange, (lonBits[0] & FIRST_BIT_FLAGGED) == FIRST_BIT_FLAGGED);
                lonBits[0] <<= 1;
            }
            isEvenBit = !isEvenBit;
        }
        hash.bits <<= (MAX_BIT_PRECISION - hash.significantBits);
        setBoundingBox(hash, latitudeRange, longitudeRange);
        hash.point = hash.boundingBox.getCenter();
        return hash;
    }
    
protected:
    void addOnBitToEnd() {
        significantBits++;
        bits <<= 1;
        bits |= 0x1;
    }

    void addOffBitToEnd() {
        significantBits++;
        bits <<= 1;
        bits &= ~(0x1);
    }

    long extractEverySecondBit(long copyOfBits, int numberOfBits) {
        long value = 0;
        for (int i = 0; i < numberOfBits; i++) {
            if ((copyOfBits & FIRST_BIT_FLAGGED) == FIRST_BIT_FLAGGED) {
                value |= 0x1;
            }
            value <<= 1;
            copyOfBits <<= 2;
        }
        value >>= 1;
        return value;
    }

    void getNumberOfLatLonBits(int* ans) {
        if (significantBits % 2 == 0) {
            ans[0] = significantBits / 2;
            ans[1] = significantBits / 2;
        } else {
            ans[0] = significantBits / 2;
            ans[1] = significantBits / 2 + 1;
        }
    }
    
public:
    void getRightAlignedLatitudeBits(long* out) {
        long copyOfBits = bits << 1;
        int* latLonBits = (int *)malloc(2 * sizeof(int));
        getNumberOfLatLonBits(latLonBits);
        long value = extractEverySecondBit(copyOfBits, latLonBits[0]);
        getNumberOfLatLonBits(latLonBits);
        out[0] = value;
        out[1] = latLonBits[0];
        
        free(latLonBits);
    }

    void getRightAlignedLongitudeBits(long* out) {
        long copyOfBits = bits;
        int* latLonBits = (int *)malloc(2 * sizeof(int));
        getNumberOfLatLonBits(latLonBits);
        long value = extractEverySecondBit(copyOfBits, latLonBits[1]);
        getNumberOfLatLonBits(latLonBits);
        out[0] = value;
        out[1] = latLonBits[1];
        
        free(latLonBits);
    }
    
    GeoHash getNorthernNeighbour() {
        long *latitudeBits = (long*)malloc(sizeof(long)*2);
        long *longitudeBits = (long*)malloc(sizeof(long)*2);
        
        getRightAlignedLatitudeBits(latitudeBits);
        getRightAlignedLongitudeBits(longitudeBits);
        
        latitudeBits[0] += 1;
        latitudeBits[0] = maskLastNBits(latitudeBits[0], latitudeBits[1]);
        GeoHash hash = recombineLatLonBitsToHash(latitudeBits, longitudeBits);
        free(latitudeBits);
        free(longitudeBits);
        return hash;
    }

    GeoHash getSouthernNeighbour() {
        long *latitudeBits = (long*)malloc(sizeof(long)*2);
        long *longitudeBits = (long*)malloc(sizeof(long)*2);
        
        getRightAlignedLatitudeBits(latitudeBits);
        getRightAlignedLongitudeBits(longitudeBits);
        
        latitudeBits[0] -= 1;
        latitudeBits[0] = maskLastNBits(latitudeBits[0], latitudeBits[1]);
        GeoHash hash = recombineLatLonBitsToHash(latitudeBits, longitudeBits);
        free(latitudeBits);
        free(longitudeBits);
        
        return hash;
    }

    GeoHash getEasternNeighbour() {
        long *latitudeBits = (long*)malloc(sizeof(long)*2);
        long *longitudeBits = (long*)malloc(sizeof(long)*2);
        
        getRightAlignedLatitudeBits(latitudeBits);
        getRightAlignedLongitudeBits(longitudeBits);
        
        longitudeBits[0] += 1;
        longitudeBits[0] = maskLastNBits(longitudeBits[0], longitudeBits[1]);
        GeoHash hash = recombineLatLonBitsToHash(latitudeBits, longitudeBits);
        free(latitudeBits);
        free(longitudeBits);
        
        return hash;
    }

    GeoHash getWesternNeighbour() {
        long *latitudeBits = (long*)malloc(sizeof(long)*2);
        long *longitudeBits = (long*)malloc(sizeof(long)*2);
        
        getRightAlignedLatitudeBits(latitudeBits);
        getRightAlignedLongitudeBits(longitudeBits);
        
        longitudeBits[0] -= 1;
        longitudeBits[0] = maskLastNBits(longitudeBits[0], longitudeBits[1]);
        GeoHash hash = recombineLatLonBitsToHash(latitudeBits, longitudeBits);
        free(latitudeBits);
        free(longitudeBits);
        
        return hash;
    }
    
    bool equals(GeoHash& obj) {
        if (obj.significantBits == significantBits && obj.bits == bits) {
            return true;
        }
        
        return false;
    }
    
    int hashCode() {
        int f = 17;
        f = 31 * f + (int) (bits ^ (bits >> 32));
        f = 31 * f + significantBits;
        return f;
    }
};

#endif /* GeoHash_hpp */
