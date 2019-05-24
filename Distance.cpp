#include <iostream>
#include "Distance.h"

using namespace std;
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
  //cout << "The value of distance is " << distance << endl;
  if (!(distance >= 0))
    throw GeographicErr("distance does not satisfy d >= 0");
  double Re = 6371.;
  distance = Re * M_PI * distance/180.;
  return distance;
}
