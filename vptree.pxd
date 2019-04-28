from libcpp.memory cimport shared_ptr
from libcpp.vector cimport vector
from libcpp.pair cimport pair

cdef extern from "VPTree.h":
    cdef cppclass VPTree:
       VPTree() except +
       VPTree(vector[shared_ptr[Point]],Distance distance) except +
       vector[pair[double,shared_ptr[Point]]] getAllInRange(shared_ptr[Point]&,double maxDistance)


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

    cdef cppclass SphericalPoint:
       SphericalPoint() except +
       double getCoordinate1()
       double getCoordinate2()