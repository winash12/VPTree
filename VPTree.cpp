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
  this->left = nullptr;
  this->right = nullptr;
  double inf = numeric_limits<double>::max();
  this->left_min = inf;
  this->left_max = 0;
  this->right_min = inf;
  this->right_max = 0;
  this->distance_function = pfunc;

  this->vp = points.front();
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
      this->geod.Inverse(vp.getCoordinate1(),vp.getCoordinate2(),lat,lon,d);
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
	  this->right_min = std::min(distance,this->right_min);
	  if (distance > this->right_max)
	    {
	      this->right_max = distance;
	      right_points.at(0)= point;
	    }
	  else
	    {
	      right_points.push_back(point);
	    }
	}
      else
	{
	  this->left_min = min(distance,this->left_min);
	  if (distance > this->left_max)
	    {
	      this->left_max = distance;
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
      this->left = new VPTree(left_points,this->distance_function);
    }
  if (right_points.size() > 0)
    {
      this->right = new VPTree(right_points,this->distance_function);
    }
}

bool VPTree::_isLeaf()
{
  if (this->left == nullptr and this->left == nullptr)
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
  
vector<Point> VPTree::getAllInRange(Point query, double maxDistance)
{
  vector<Point> neighbors;
  vector<Point> nodes_to_visit;

  while (nodes_to_visit.size() >0)
    {
      
    }
  return neighbors;
}


  
