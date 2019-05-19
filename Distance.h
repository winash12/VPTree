#ifndef DISTANCE_H
#define DISTANCE_H

#define _USE_MATH_DEFINES

#include <math.h>
#include <memory>
#include <GeographicLib/Geodesic.hpp>

#include "Point.h"

using namespace GeographicLib;

using std::shared_ptr;


class Distance
{
 protected:
  double distance;
 public:
  virtual double calculateDistance(shared_ptr<Point> point1, shared_ptr<Point> point2);
};


class GreatCircleDistance : public Distance
{
 private:
  const Geodesic geod = Geodesic(180./M_PI,0);
 public:
  GreatCircleDistance(){};
  double calculateDistance(shared_ptr<Point> point1, shared_ptr<Point> point2);
};

#endif /* DISTANCE_H */
