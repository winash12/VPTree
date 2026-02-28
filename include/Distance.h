#ifndef DISTANCE_H
#define DISTANCE_H

#include "Point.h"
#include <GeographicLib/Geodesic.hpp>

/**
 * Distance Interface
 * Updated to use pointers to match your VPTree's .get() calls.
 */
class Distance {
public:
    virtual ~Distance() {}
    // Pure virtual to ensure sub-classes implement it
    virtual double calculate(const Point* p1, const Point* p2) const = 0;
};

/**
 * The "Karney" Implementation
 * We use your 'GreatCircleDistance' name but power it with GeographicLib 
 * for ellipsoid accuracy (essential for GPM PR).
 */
class GreatCircleDistance : public Distance {
private:
    const GeographicLib::Geodesic& geod;

public:
    GreatCircleDistance() : geod(GeographicLib::Geodesic::WGS84()) {}

  double calculate(const Point* p1, const Point* p2) const override {
    double s12;
    // By passing 5 arguments, GeographicLib calculates just the distance.
    // The mask 'GeographicLib::Geodesic::DISTANCE' is used internally by this overload.
    geod.Inverse(p1->lat, p1->lon, p2->lat, p2->lon, s12);
    return s12;
  }
  
};

/**
 * Euclidean Implementation
 * Used for flat-map approximations or debugging.
 */
class EuclideanDistance : public Distance {
public:
    double calculate(const Point* p1, const Point* p2) const override {
        double dlat = p1->lat - p2->lat;
        double dlon = p1->lon - p2->lon;
        return std::sqrt(dlat * dlat + dlon * dlon);
    }
};

#endif


