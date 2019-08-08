from libcpp.deque cimport deque
from libcpp.pair cimport pair

cdef extern from "<utility>" namespace "std" nogil:
  deque[Point]&& move(deque[Point]&&)

cdef extern from "VPTree.h":
    cdef cppclass VPTree:
       VPTree() nogil except +
       #deque[pair[double,Point]] getAllInRange(Point point,double maxDistance) nogil
       deque[pair[double,Point]] getAllInRange(Point point,double maxDistance)
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
       double getCoordinate1() nogil
       double getCoordinate2() nogil 
       void setCoordinate1(double coordinate1) nogil
       void setCoordinate2(double coordinate2) nogil
       
    cdef cppclass SphericalPoint(Point):
       SphericalPoint() nogil except +
       double getCoordinate1() nogil 
       double getCoordinate2() nogil 
       void setCoordinate1(double lat) nogil 
       void setCoordinate2(double lon) nogil
