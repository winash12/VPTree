# distutils: language = c++
#

import numpy as np
cimport numpy as np

from libc.stdio cimport printf
from libcpp.memory cimport shared_ptr,unique_ptr,make_shared,make_unique,dynamic_pointer_cast
from libcpp.vector cimport vector
from libcpp.deque cimport deque
from libcpp.pair cimport pair


from cython.operator cimport dereference
from cython.operator cimport postincrement as inc

from vptree cimport VPTree
from vptree cimport Distance
from vptree cimport Point
from vptree cimport SphericalPoint
from vptree cimport GreatCircleDistance


cdef class PyVPTree:
    cdef VPTree *vptree
    cdef deque[Point] points
    cdef Distance *gcd
    
    def __cinit__(self):
        self.vptree = new VPTree()
        self.points = deque[Point]()

    def initializeGreatCircleDistance(self):

        self.gcd = new GreatCircleDistance()
        self.vptree.initializeDistance(self.gcd)
        
    def buildPointsVector(self,np.ndarray[np.float64_t,ndim=2] points):

        cdef SphericalPoint spoint
        cdef Point point
        for i in range(0,len(points)):
            spoint = SphericalPoint()
            point = <Point>spoint
            point.setCoordinate1(points[i,0])
            point.setCoordinate2(points[i,1])
            self.points.push_front(point)

    def initializePoints(self):
        self.vptree.initializeVPTreePoints(self.points)

    def getAllInRange(self,np.float64_t latitude,np.float64_t longitude, np.float64_t maxDistance):
        cdef vector[pair[double,Point]] vec
        cdef SphericalPoint spoint
        cdef Point point
        spoint = SphericalPoint()
        point = <Point>spoint
        point.setCoordinate1(latitude)
        point.setCoordinate2(longitude)
        vec = self.vptree.getAllInRange(point,maxDistance)
        cdef vector[pair[double,Point]].iterator it = vec.begin()
        result = []
        while it != vec.end():
            distance = dereference(it).first
            p = dereference(it).second
            lat = p.getCoordinate1()
            lon = p.getCoordinate2()
            p1 = np.append([lat,lon])
            result.append(tuple((distance,p1)))
        return result
        
    def __dealloc__(self):
        if self.vptree != NULL:
            del self.vptree
