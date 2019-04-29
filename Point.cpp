#include "Point.h"

double SphericalPoint::getCoordinate1()
{
  return latitude;
}

double SphericalPoint::getCoordinate2()
{
  return longitude;
}


void SphericalPoint::setCoordinate1(double lat)
{
  latitude = lat;
}


void SphericalPoint::setCoordinate2(double lon)
{
  longitude = lon;
}

