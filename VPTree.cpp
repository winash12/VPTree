#include "VPTree.h"
#include <algorithm>
#include <limits>

using std::vector;
using std::shared_ptr;
using std::pair;
using std::make_pair;
using std::numeric_limits;

// Thread-local scratchpad to avoid heap allocations during parallel Cython calls
thread_local std::vector<const VPTree*> nodes_to_visit;

VPTree::VPTree() : 
    left(nullptr), 
    right(nullptr), 
    distance(nullptr), 
    left_min(numeric_limits<double>::max()), 
    left_max(0), 
    right_min(numeric_limits<double>::max()), 
    right_max(0) 
{}
void VPTree::initializeVPTreePoints(vector<shared_ptr<Point>>& points) {
    if (points.empty()) return;

    // 1. Vantage Point Selection (O(1) from back)
    vp = points.back();
    points.pop_back();

    if (points.empty()) return;

    // 2. Distance Calculation for Partitioning
    vector<double> distances;
    distances.reserve(points.size());
    for (const auto& p : points) {
        distances.push_back(distance->calculate(vp.get(), p.get()));
    }

    // 3. Find Median
    vector<double> dists_copy = distances;
    double median = _findMedian(dists_copy);

    vector<shared_ptr<Point>> left_points, right_points;
    left_points.reserve(points.size() / 2);
    right_points.reserve(points.size() / 2);
    // 4. Partition and Bound Tracking
    for (size_t i = 0; i < points.size(); ++i) {
        double d = distances[i];
        if (d < median) {
            left_min = std::min(d, left_min);
            left_max = std::max(d, left_max);
            left_points.push_back(points[i]);
        } else {
            right_min = std::min(d, right_min);
            right_max = std::max(d, right_max);
            right_points.push_back(points[i]);
        }
    }
    // 5. Recursive Build
    if (!left_points.empty()) {
      left = std::make_shared<VPTree>();
      left->initializeDistance(distance);
      left->initializeVPTreePoints(left_points);
    }
    
    if (!right_points.empty()) {
      right = std::make_shared<VPTree>();
      right->initializeDistance(distance);
        right->initializeVPTreePoints(right_points);
    }
}
double VPTree::_findMedian(vector<double>& distances) const {
    if (distances.empty()) return 0.0;
    size_t n = distances.size() / 2;
    std::nth_element(distances.begin(), distances.begin() + n, distances.end());
    return distances[n];
}

std::vector<std::pair<double, std::shared_ptr<Point>>> VPTree::getAllInRange(const Point* query, double maxDistance) const {
  std::vector<std::pair<double, std::shared_ptr<Point>>> neighbors;
  
  nodes_to_visit.clear();
  nodes_to_visit.push_back(this);
  
  while (!nodes_to_visit.empty()) {
    const VPTree* node = nodes_to_visit.back();
    nodes_to_visit.pop_back();
    
    if (!node || !node->vp) continue;
    
    // Fixed to call calculate (matching your build logic)
    double dist = distance->calculate(query, node->vp.get());
    
    if (dist <= maxDistance) {
      neighbors.push_back({dist, node->vp});
    }
    
    // Your specific min/max pruning logic - preserved
    if (node->right && dist + maxDistance >= node->right_min && dist - maxDistance <= node->right_max) {
      nodes_to_visit.push_back(node->right.get());
    }
    if (node->left && dist + maxDistance >= node->left_min && dist - maxDistance <= node->left_max) {
      nodes_to_visit.push_back(node->left.get());
    }
  }
  return neighbors; // MOVED OUTSIDE THE WHILE LOOP
}

bool VPTree::_isLeaf() {
  return (left == nullptr && right == nullptr);
}
