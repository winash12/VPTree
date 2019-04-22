#include <GeographicLib/Geodesic.hpp>

#include "Point.h"

using namespace GeographicLib;

class Distance
{
 private:
  double distance;
 public:
  double calculateDistance(Point point1, Point point2);
};


class GreatCircleDistance : public Distance
{
 private:
  double distance;
  const Geodesic& geod = Geodesic::WGS84();
  
 public:
  double calculateDistance(SphericalPoint point1,SphericalPoint point2);

};
