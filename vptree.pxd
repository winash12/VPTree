from libcpp.memory cimport shared_ptr
from libcpp.vector cimport vector
from libcpp.pair cimport pair

cdef extern from "VPTree.h":
    cdef cppclass VPTree:
       VPTree() except +
       vector[pair[double,shared_ptr[Point]]] getAllInRange(shared_ptr[Point]&,double maxDistance)
       void initializeVPTreePoints(vector[shared_ptr[Point]] *points)
       void initializeDistance(Distance *distance)
       
cdef extern from "Distance.h" :
    cdef cppclass Distance:
       Distance() except +
       double calculateDistance(shared_ptr[Point]&,shared_ptr[Point]&)	
    cdef cppclass GreatCircleDistance(Distance):
       GreatCircleDistance() except +
       double calculateDistance(SphericalPoint point1, SphericalPoint2)
											

cdef extern from "Point.h":
    cdef cppclass Point:
       Point() except +
       double getCoordinate1()
       double getCoordinate2()
       void setCoordinate1(double coordinate1)
       void setCoordinate2(double coordinate2)
       
    cdef cppclass SphericalPoint(Point):
       SphericalPoint() except +
       double getCoordinate1()
       double getCoordinate2()
       void setCoordinate1(double lat)
       void setCoordinate2(double lon)
