#include "Point.h"

double Point::getCoordinate1() const
{
  return coordinate1;
}

double Point::getCoordinate2() const
{
  return coordinate2;
}


void Point::setCoordinate1(double lat)
{
  coordinate1 = lat;
}


void Point::setCoordinate2(double lon)
{
  coordinate2 = lon;
}


double SphericalPoint::getCoordinate1() const
{
  return latitude;
}

double SphericalPoint::getCoordinate2() const
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


