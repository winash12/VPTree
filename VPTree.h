#ifndef VPTREE_H
#define VPTREE_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <utility>
#include <deque>
#include <string.h>
#include <memory>
#include <boost/optional.hpp>
#include <vector>
#include <deque>
#include "Distance.h"

using std::vector;
using std::deque;
using std::pair;



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
  Point _selectVantagePoint(deque<Point> points);
 public:

  VPTree(){};
  friend bool operator!= (Point &lhs,Point &rhs);
  void initializeVPTreePoints(std::deque<Point> points);


  std::vector<std::pair<double,Point>> getAllInRange(Point query ,double maxDistance);
  std::vector<std::vector<std::pair<double,Point>>> getAllInRange (std::vector<Point> queryPoints,double maxDistance);
  template<typename T>
    inline void initializeDistance(T&& distFunc)
  {
    distance = distFunc;
  }
};
#endif /* VPTREE_H */
