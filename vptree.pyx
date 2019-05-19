#cython: language_level=3, boundscheck=True
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
    cdef Distance *gcd
    cdef deque[shared_ptr[Point]] points

    def __cinit__(self):
        self.vptree = new VPTree()
        self.points = deque[shared_ptr[Point]]()

        
    def buildPointsVector(self,np.ndarray[np.float64_t,ndim=1] latitude ,np.ndarray[np.float64_t,ndim=1] longitude):

        cdef shared_ptr[Point] point
        cdef shared_ptr[SphericalPoint] sphericalPoint
        cdef int ic,jc
        for i in range(0,len(latitude)):
            for j in range(0,len(longitude)):
                sphericalPoint = make_shared[SphericalPoint]()
                point = dynamic_pointer_cast[Point,SphericalPoint](sphericalPoint)
                dereference(point).setCoordinate1(latitude[i])
                dereference(point).setCoordinate2(longitude[j])
                #printf("%f\t%f\n",dereference(point).getCoordinate1(),dereference(point).getCoordinate2())
                self.points.push_front(point)
            
    def initializePoints(self):
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
