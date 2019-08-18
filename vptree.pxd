from libcpp.vector cimport vector
from libcpp.deque cimport deque

from libcpp.pair cimport pair

cdef extern from "<utility>" namespace "std" nogil:
  vector[pair[double,Point]]&& move(vector[pair[double,Point]]&&)

cdef extern from "VPTree.h":
    cdef cppclass VPTree:
       VPTree() nogil except +
       vector[pair[double,Point]] getAllInRange(Point point,double maxDistance) nogil
       void initializeVPTreePoints(deque[Point] points) 
       void initializeDistance(Distance *distance) 
       
cdef extern from "Distance.h" :
    cdef cppclass Distance:
       Distance() except +
       double calculateDistance(Point point1,Point point2) 
    cdef cppclass GreatCircleDistance(Distance):
       GreatCircleDistance()  except +
       double calculateDistance(Point point1,Point point2)
											

cdef extern from "Point.h":
    cdef cppclass Point:
       Point() nogil except +
       double getCoordinate1() 
       double getCoordinate2()  
       void setCoordinate1(double coordinate1) nogil
       void setCoordinate2(double coordinate2) nogil
       
    cdef cppclass SphericalPoint(Point):
       SphericalPoint() nogil except +
       double getCoordinate1()  
       double getCoordinate2()  
       void setCoordinate1(double lat) nogil 
       void setCoordinate2(double lon) nogil
