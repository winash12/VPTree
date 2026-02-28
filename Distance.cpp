#include <iostream>
#include "Distance.h"

using namespace GeographicLib;

double Distance::calculateDistance(const Point& point1, const Point& point2)
{
  return distance;
}


double GreatCircleDistance::calculateDistance(const Point& point1,const Point& point2)
{
  double lat2 = point2.getCoordinate1();
  double lon2 = point2.getCoordinate2();
  //cout << "The value of lat is " << lat2 << endl;
  //cout << "The value of lon is " << lon2 << endl;
  double lat1 = point1.getCoordinate1();
  double lon1 =  point1.getCoordinate2();
  //cout << "The value of lat is " << lat1 << endl;
  //cout << "The value of lon is " << lon1 << endl;

  geod.Inverse(lat1,lon1,lat2,lon2,distance);

  if (!(distance >= 0))
    throw GeographicErr("distance does not satisfy d >= 0");
  distance = deg2km(distance);
  //cout << "The value of distance is " << distance << endl;
  return distance;
}

double GreatCircleDistance::deg2km(double d)
{
  double Re = 6371.;
  double km;
  km = (d * M_PI * Re)/180.;
  return km;
}
