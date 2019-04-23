#include <iostream>
#include <exception>
#include <limits>
#include <fstream>
#include <vector>
#include <boost/optional.hpp>
#include <boost/range/combine.hpp>
#include <list>
#include <typeinfo>
#include "VPTree.h"

using namespace std;
using std::vector;


VPTree::VPTree(vector<Point> points,Distance pfunc)
{
  left = nullptr;
  right = nullptr;
  double inf = numeric_limits<double>::max();
  left_min = inf;
  left_max = 0;
  right_min = inf;
  right_max = 0;
  distance = pfunc;

  vp = points.front();
  //points.erase(points.begin()+0);
  points.pop_back();
  if (points.size() == 0)
    {
      return;
    }
  vector<Point>::iterator it;
  vector<double> distances;
  for (it = points.begin();it != points.end();++it)
    {
      double d;
      Point point = *it;
      d = distance.calculateDistance(vp,point);
      distances.push_back(d);
    }
  double median = _findMedian(distances);
  vector<Point> left_points,right_points;
  for (auto tup: boost::combine(points,distances))
    {
      Point point;
      double distance;
      boost::tie(point,distance) = tup;
      if (distance >= median)
	{
	  right_min = std::min(distance,right_min);
	  if (distance > right_max)
	    {
	      right_max = distance;
	      right_points.at(0)= point;
	    }
	  else
	    {
	      right_points.push_back(point);
	    }
	}
      else
	{
	  left_min = min(distance,left_min);
	  if (distance > left_max)
	    {
	      left_max = distance;
	      left_points.at(0) = point;
	    }
	  else
	    {
	      left_points.push_back(point);
	    }
	}
    }
  if (left_points.size() > 0)
    {
      left = new VPTree(left_points,distance);
    }
  if (right_points.size() > 0)
    {
      right = new VPTree(right_points,distance);
    }
}

bool VPTree::_isLeaf()
{
  if (left == nullptr and right == nullptr)
    {
      return true;
    }
  else
    {
      return false;
    }
}

double VPTree::_findMedian(vector<double>distances) 
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
  
vector<pair<double,Point>> VPTree::getAllInRange(Point query, double maxDistance)
{
  vector<pair<double,Point>> neighbors;
  vector<pair<VPTree*,double>> nodes_to_visit;
  VPTree* node;
  double d0;
  nodes_to_visit.push_back(make_pair(this,0));
  while (nodes_to_visit.size() >0)
    {
      auto it  = nodes_to_visit.end();
      node = it->first;
      d0 = it->second;
      if (node == nullptr or d0 > maxDistance)
	continue;
      Point point = node->vp;

      double dist = distance.calculateDistance(query,point);
      if (dist < maxDistance)
	neighbors.push_back(make_pair(dist,node->vp));
      
      if (node->_isLeaf())
	continue;
      if (node->left_min <= dist && dist <= node->left_max)
	{
	  nodes_to_visit.insert(nodes_to_visit.begin(),make_pair(node->left,0));
	}
      else if (node->left_min-maxDistance <= dist && dist <= node->left_max + maxDistance )
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
	  nodes_to_visit.insert(nodes_to_visit.begin(),make_pair(node->right,0));
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


  
