#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <vector>
#include <string.h>
#include <boost/optional.hpp>

#include "Distance.h"

class VPTree
{
 private:
  VPTree *left,*right;
  double left_min,right_min,left_max,right_max;
  Point vp;
  //Distance function
  Distance distance;

  bool _isLeaf();

  double _findMedian(std::vector<double>distances);
  
 public:

  VPTree(std::vector<Point> points,Distance distance);



  std::vector<std::pair<double,Point>> getAllInRange(Point query ,double maxDistance);
};
