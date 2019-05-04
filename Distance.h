#ifndef DISTANCE_H
#define DISTANCE_H

#include <memory>
#include <GeographicLib/Geodesic.hpp>

#include "Point.h"

using namespace GeographicLib;

using std::shared_ptr;


class Distance
{
 private:
  double distance;
 public:
  double calculateDistance(shared_ptr<Point> point1, shared_ptr<Point> point2);
};


class GreatCircleDistance : public Distance
{
 private:
  double distance;
  const Geodesic& geod = Geodesic::WGS84();
  
 public:
  double calculateDistance(SphericalPoint point1,SphericalPoint point2);

};

#endif /* DISTANCE_H */
