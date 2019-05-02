#include "Distance.h"


using namespace GeographicLib;

double Distance::calculateDistance(shared_ptr<Point> point1, shared_ptr<Point> point2)
{
  return distance;
}

double GreatCircleDistance::calculateDistance(SphericalPoint point1,SphericalPoint point2)
{
  double lat = point2.getCoordinate1();
  double lon = point2.getCoordinate2();
  geod.Inverse(point1.getCoordinate1(),point1.getCoordinate2(),lat,lon,distance);
  return distance;
}
