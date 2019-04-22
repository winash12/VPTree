#include "Distance.h"


using namespace GeographicLib;

double Distance::calculateDistance(Point point1, Point point2)
{
  return distance;
}

double GreatCircleDistance::calculateDistance(SphericalPoint point1,SphericalPoint point2)
{
  double lat = point1.getCoordinate1();
  double lon = point2.getCoordinate2();
  geod.Inverse(point1.getCoordinate1(),point1.getCoordinate2(),lat,lon,distance);
  return distance;
}
