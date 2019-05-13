#cython: language_level=3, boundscheck=True
# distutils: language = c++
#

import numpy as np
cimport numpy as np

from libc.stdio cimport printf
from libcpp.memory cimport shared_ptr
from libcpp.vector cimport vector
from libcpp.pair cimport pair


from cython.operator cimport dereference

from vptree cimport VPTree
from vptree cimport Distance
from vptree cimport Point
from vptree cimport SphericalPoint
from vptree cimport GreatCircleDistance


cdef class PyVPTree:
    cdef VPTree *vptree
    cdef Distance *gcd
    cdef vector[shared_ptr[Point]] points

    def __cinit__(self):
        self.vptree = new VPTree()
        self.points = vector[shared_ptr[Point]]()

        
    def buildPointsVector(self,np.ndarray[np.float64_t,ndim=1] latitude ,np.ndarray[np.float64_t,ndim=1] longitude):

        cdef shared_ptr[Point] point
        for i in range(latitude.shape[0]):
            for j in range(longitude.shape[0]):
                point = shared_ptr[Point](new SphericalPoint())
                dereference(point).setCoordinate1(latitude[i])
                dereference(point).setCoordinate2(longitude[j])
                self.points.push_back(point)
        
    cdef initializePoints(self):
        self.vptree.initializeVPTreePoints(self.points)
    
        
    def initializeGreatCircleDistance(self):
        self.gcd = new GreatCircleDistance()
        self.vptree.initializeDistance(self.gcd)

    def getAllInRange(self,np.float64_t latitude,np.float64_t longitude, np.float64_t maxDistance):
        cdef shared_ptr[Point] point
        cdef vector[pair[double,shared_ptr[Point]]] vec

        point = shared_ptr[Point](new SphericalPoint())
        dereference(point).setCoordinate1(latitude)
        dereference(point).setCoordinate2(longitude)
        vec = self.vptree.getAllInRange(point,maxDistance)
        cdef vector[pair[double,shared_ptr[Point]]].iterator it = vec.begin()
        result = []
        while it != vec.end():
            distance = dereference(it).first
            p = dereference(it).second
            lat = dereference(p).getCoordinate1()
            lon = dereference(p).getCoordinate2()
            p1 = np.append([lat,lon])
            result.append(tuple((distance,p1)))
        return result
        
    def __dealloc__(self):
        if self.vptree != NULL:
            del self.vptree
