#include <iostream>
#include "Distance.h"

using namespace std;
using namespace GeographicLib;

double Distance::calculateDistance(shared_ptr<Point> point1, shared_ptr<Point> point2)
{
  return distance;
}

double GreatCircleDistance::calculateDistance(shared_ptr<Point> point1,shared_ptr<Point> point2)
{
  double lat = point1->getCoordinate1();
  double lon = point2->getCoordinate2();
  geod.Inverse(point1->getCoordinate1(),point1->getCoordinate2(),lat,lon,distance);
  if (!(distance >= 0))
    throw GeographicErr("distance does not satisfy d >= 0");
  return distance;
}
