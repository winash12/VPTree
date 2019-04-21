#include <iostream>
#include <exception>
#include <limits>
#include <fstream>
#include <vector>
#include <boost/optional.hpp>
#include <boost/range/combine.hpp>
#include <list>
#include "VPTree.h"

using namespace std;
using std::vector;


VPTree::VPTree(vector<Point> points,dist_func pfunc)
{
  left = nullptr;
  right = nullptr;
  double inf = numeric_limits<double>::max();
  left_min = inf;
  left_max = 0;
  right_min = inf;
  right_max = 0;
  distance_function = pfunc;

  vp = points.front();
  points.erase(points.begin()+0);

  if (points.size() == 0)
    {
      return;
    }
  vector<Point>::iterator it;
  vector<double> distances;
  for (it = points.begin();it != points.end();++it)
    {
      double lat = it->getCoordinate1();
      double lon = it->getCoordinate2();
      double d;
      geod.Inverse(vp.getCoordinate1(),vp.getCoordinate2(),lat,lon,d);
      distances.push_back(d);
    }
  double median = findMedian(distances);
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
      left = new VPTree(left_points,distance_function);
    }
  if (right_points.size() > 0)
    {
      right = new VPTree(right_points,distance_function);
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

double VPTree::findMedian(vector<double>distances) 
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
    
      double distance = distance_function(query,node->vp);
      if (distance < maxDistance)
	neighbors.push_back(make_pair(distance,node->vp));
      
      if (node->_isLeaf())
	continue;
      if (node->left_min <= distance && distance <= node->left_max)
	{
	  nodes_to_visit.insert(nodes_to_visit.begin(),make_pair(node->left,0));
	}
      else if (node->left_min-maxDistance <= distance && distance <= node->left_max + maxDistance )
	{
	  double dd;
	  if (distance < node->left_min)
	    dd = node->left_min - distance;
	  else
	    dd = distance - node->left_max;
	  nodes_to_visit.push_back(make_pair(node->left,dd));
	}
      if (node->right_min <= distance && distance <= node->right_max)
	{
	  nodes_to_visit.insert(nodes_to_visit.begin(),make_pair(node->right,0));
	}
      else if (node->right_min-maxDistance <= distance && distance <= node->right_max + maxDistance )
	{
	  double dd;
	  if (distance < node->right_min)
	    dd = node->right_min - distance;
	  else
	    dd = distance - node->right_max;
	  nodes_to_visit.push_back(make_pair(node->right,dd));
	}
      
    }	     
  return neighbors;
}


  
