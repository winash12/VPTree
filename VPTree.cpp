#include <time.h>
#include <iostream>
#include <exception>
#include <limits>
#include <fstream>
#include <deque>
#include <list>
#include <boost/optional.hpp>
#include <boost/range/combine.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <typeinfo>
#include <memory>
#include <xtensor/xarray.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xadapt.hpp>
#include "VPTree.h"

using namespace std;
using std::vector;
using std::deque;
using namespace boost::accumulators;

void VPTree::initializeDistance(Distance *pfunc)
{
  distance = pfunc;
}

void VPTree::initializeVPTreePoints(deque<Point> points)
{
  time_t start,end;
  left = nullptr;
  right = nullptr;
  double inf = numeric_limits<double>::max();
  left_min = inf;
  left_max = 0;
  right_min = inf;
  right_max = 0;

  time(&start);
  vp = points.front();
  points.pop_front();

  if (points.size() == 0)
    {
      return;
    }
  deque<Point>::iterator it;
  deque<double> distances;
  xt::xarray<double> xdistances;
  try
    {
      for (it = points.begin();it != points.end();++it)
	{
	  double d;
	  Point point = *it;
	  d = distance->calculateDistance(vp,point);
	  distances.push_back(d);
	  //xdistances.cend(d);
	}
    }
  catch (const std::out_of_range& oor)
    {
      std::cerr <<"Out of Range error: " << oor.what() << endl;
      exit(0);
    }


  double median = xt::median(xt::adapt(distances));
  //cout <<"The value of median is " << median << endl;
  time(&end);

  deque<Point> left_points,right_points;
  try
    {
      for (auto tup: boost::combine(points,distances))
	{
	  Point point;
	  double dist;
	  boost::tie(point,dist) = tup;
	  if (dist >= median)
	    {
	      right_min = std::min(dist,right_min);
	      if (dist > right_max)
		{
		  right_max = dist;
		  right_points.push_front(point);
		  
		}
	      else
		{
		  right_points.push_back(point);
		}
	    }
	  else
	    {
	      left_min = std::min(dist,left_min);
	      if (dist > left_max)
		{
		  left_max = dist;
		  left_points.push_front(point);
		}
	      else
		{
		  left_points.push_back(point);
		}
	    }
	}
    }
  catch (const std::out_of_range& oor)
    {
      std::cerr <<"Out of Range error: " << oor.what() << endl;
    }
  if (left_points.size() > 0)
    {
      this->left = new VPTree();
      this->left->initializeDistance(this->distance);
      this->left->initializeVPTreePoints(left_points);

    }
  if (right_points.size() > 0)
    {
      this->right = new VPTree();
      this->right->initializeDistance(this->distance);
      this->right->initializeVPTreePoints(right_points);
    }
}

bool VPTree::_isLeaf()
{
  if (!(left)  and !(right))
    {
      return true;
    }
  else
    {
      return false;
    }
}

double VPTree::_findMedian(deque<double>distances) 
{
  size_t size = distances.size();
  
  if (size == 0)
    {
      return 0;
    }
  else
    {
  // First we sort the array 
      sort(distances.begin(),distances.end()); 
      
      if (size % 2 == 0)
	{
	  return (distances[size/2 -1] + distances[size/2])/2;
	}
      else
	{
	  return distances[size/2];
	} 
    }
}

std::vector<std::deque<std::pair<double,Point>>> VPTree::getAllInRange(std::deque<Point> queryPoints,double maxDistance)
{
  std::deque<Point>::iterator it;

  std::vector<std::deque<std::pair<double,Point>>> neighborCollection;
  try
    {
      for (it = queryPoints.begin();it != queryPoints.end();++it)
	{
	  Point query = *it;
	  deque<pair<double,Point>> neighbors;
	  neighbors = getAllInRange(query,maxDistance);
	  neighborCollection.push_back(neighbors);
	}
    }
  catch(const std::out_of_range& oor)
    {
      exit(0);
    }
  cout << "The lengt of nc is " << size(neighborCollection) << endl;
  return neighborCollection;
}

//NPY_BEGIN_ALLOW_THREADS
//NPY_END_ALLOW_THREADS
//Similar to Py_BEGIN_ALLOW_THREADS
// And Py_END_ALLOW_THREADS
deque<pair<double,Point>> VPTree::getAllInRange(Point query, double maxDistance)
{
  deque<pair<double,Point>> neighbors;
  deque<pair<VPTree*,double>> nodes_to_visit;
  VPTree *node;
  double d0;
  nodes_to_visit.push_front(make_pair(this,0));

  while (nodes_to_visit.size() > 0 )
    {
      deque<pair<VPTree*,double>>::iterator it = nodes_to_visit.begin();
      node = it->first;
      d0 = it->second;
      nodes_to_visit.erase(it);
      if (node == nullptr or d0 > maxDistance)
	continue;
      Point point = node->vp;

      double dist = distance->calculateDistance(query,point);
      if (dist < maxDistance)
	{
	  neighbors.push_back(make_pair(dist,point));
	}
      if (node->_isLeaf())
	continue;
      if (node->left_min <= dist && dist <= node->left_max)
	{
	  nodes_to_visit.push_front(make_pair(node->left,0));
	}
      else if (node->left_min-maxDistance <= dist && dist <= (node->left_max + maxDistance) )
	{
	  double dd;
	  if (dist < node->left_min)
	    dd = node->left_min - dist;
	  else
	    dd = dist - node->left_max;
	  nodes_to_visit.push_back(make_pair(node->left,dd));
	}
      if (node->right_min <= dist && dist <= node->right_max)
	{
	  nodes_to_visit.push_front(make_pair(node->right,0));
	}
      else if (node->right_min-maxDistance <= dist && dist <= node->right_max + maxDistance )
	{
	  double dd;
	  if (dist < node->right_min)
	    dd = node->right_min - dist;
	  else
	    dd = dist - node->right_max;
	  nodes_to_visit.push_back(make_pair(node->right,dd));
	}
    }
  return neighbors;
}

  
