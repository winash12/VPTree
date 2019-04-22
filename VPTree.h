#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <vector>
#include <string.h>
#include <boost/optional.hpp>
#include <GeographicLib/Geodesic.hpp>
#include "Point.h"

using namespace GeographicLib;


typedef std::function<double(Point point1, Point point2)> dist_func;


class VPTree
{
 private:
  //boost::optional<double> left,right;
  VPTree *left,*right;
  double left_min,right_min,left_max,right_max;
  Point vp;
  dist_func distance_function;
  const Geodesic& geod = Geodesic::WGS84();

  bool _isLeaf();
  
 public:

  VPTree(std::vector<Point> points,dist_func pfunc);

  double findMedian(std::vector<double>distances);

  std::vector<std::pair<double,Point>> getAllInRange(Point query ,double maxDistance);
};
