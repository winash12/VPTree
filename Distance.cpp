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
  double lat2 = point2->getCoordinate1();
  double lon2 = point2->getCoordinate2();
  //cout << "The value of lat is " << lat2 << endl;
  //cout << "The value of lon is " << lon2 << endl;
  double lat1 = point1->getCoordinate1();
  double lon1 =  point1->getCoordinate2();
  //cout << "The value of lat is " << lat1 << endl;
  //cout << "The value of lon is " << lon1 << endl;

  geod.Inverse(lat1,lon1,lat2,lon2,distance);
  //cout << "The value of distance is " << distance << endl;
  if (!(distance >= 0))
    throw GeographicErr("distance does not satisfy d >= 0");
  return distance;
}
