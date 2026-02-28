#ifndef VPTREE_H
#define VPTREE_H

#include <vector>
#include <memory>
#include <algorithm>
#include <utility>
#include "Point.h"
#include "Distance.h"

/**
 * High-Performance Vantage Point Tree
 * Designed for parallel search (nogil) via thread_local stacks.
 */
class VPTree {
private:
    // Pointers for the tree structure (Matches your .cpp logic)
  Distance *distance;
  std::shared_ptr<VPTree> left;
  std::shared_ptr<VPTree> right;
  
  // Bounding intervals for pruning
  double left_min, left_max;
  double right_min, right_max;
  
  // The Vantage Point for this node
  std::shared_ptr<Point> vp;
  
  // Distance metric (GreatCircle, Euclidean, etc.)
  
  
  // Internal helper for median calculation
  double _findMedian(std::vector<double>& distances) const;
  bool _isLeaf();

public:
  VPTree();
  virtual ~VPTree() = default;
  
  // Configuration
  void initializeDistance(Distance *pfunc);
  
  // Tree Building (Matches your .cpp and Cython)
  void initializeVPTreePoints(std::vector<std::shared_ptr<Point>>& points);
  
  // THE SEARCH: Marked const and nogil-friendly
  // Returns pairs of (distance, Point) as requested by your Cython logic
  std::vector<std::pair<double, std::shared_ptr<Point>>> getAllInRange(
                                                                       const Point* query, 
                                                                       double maxDistance
                                                                       ) const;
};

#endif
