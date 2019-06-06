#ifndef VPTREE_H
#define VPTREE_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <deque>
#include <string.h>
#include <memory>
#include <boost/optional.hpp>

#include "Distance.h"

using std::unique_ptr;
using std::shared_ptr;


class VPTree
{
 private:

  VPTree *left;
  VPTree *right;
  double left_min,right_min,left_max,right_max;
  Point vp;
  //Distance function
  Distance *distance;

  bool _isLeaf();

  double _findMedian(std::deque<double>distances);
  
 public:

  VPTree(){};
  
  void initializeVPTreePoints(std::deque<Point> points);
  void initializeDistance(Distance *distance);

  std::deque<std::pair<double,Point>> getAllInRange(Point query ,double maxDistance);
};
#endif /* VPTREE_H */
