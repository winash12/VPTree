# distutils: language = c++
#
import sys
import time
import numpy as np
cimport numpy as np

cimport cython

import psutil
from concurrent.futures import ThreadPoolExecutor
from functools import partial

from libc.stdio cimport printf
from libcpp.deque cimport deque
from libcpp.vector cimport vector
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
            #printf("%f\t%f\n",point.getCoordinate1(),point.getCoordinate2())
            self.points.push_back(point)

    def initializePoints(self):
        self.vptree.initializeVPTreePoints(self.points)
    
    def getNeighborsInRange(self,np.ndarray[np.float64_t,ndim=2] gridPoints,np.float64_t maxDistance):

        number_of_processors = psutil.cpu_count(logical=False)
        chunk = np.array_split(gridPoints,number_of_processors,axis=0)
        x = ThreadPoolExecutor(max_workers=number_of_processors) 
        t0 = time.time()
        func = partial(self.getNeighborsInRangeChunk,maxDistance)
        results = x.map(func,chunk)
        t1 = time.time()
        print(t1-t0)

        return results

    def getNeighborsInRangeChunk(self,np.float64_t maxDistance,np.ndarray[np.float64_t,ndim=2] gridPoints):
        print(maxDistance)
        cdef int i
        cdef deque[pair[double,Point]] deq
        cdef double[:,:] gPoints
        cdef vector[pair[double,Point]] result
        cdef vector[vector[pair[double,Point]]] accumulatedResult
        gPoints = memoryview(gridPoints)
        resultList = []
        with nogil:
            for i in range(gPoints.shape[0]):
                deq = self.getNeighborsInRangeForSingleQueryPoint(gPoints[i],maxDistance)
                #result = self.processResult(deq)
                #accumulatedResult.push_back(result)
                i = 0
        return resultList

    @cython.boundscheck(True)
    cdef vector[pair[double,Point]] processResult(self,deque[pair[double,Point]] deq) :

        cdef deque[pair[double,Point]].iterator it = deq.begin()
        cdef double distance
        cdef Point p
        cdef pair[double,Point] entry
        cdef vector[pair[double,Point]] result  
        while it != deq.end():
            distance = dereference(it).first
            p = dereference(it).second
            entry.first = distance
            entry.second = p
            result.push_back(entry)
            inc(it)
        return result

    @cython.boundscheck(True)
    cdef deque[pair[double,Point]] getNeighborsInRangeForSingleQueryPoint(self,double[:] qpoint, double  maxDistance) nogil :
        cdef deque[pair[double,Point]] deq
        cdef SphericalPoint spoint
        cdef Point point

        spoint = SphericalPoint() 
        point = <Point>spoint
        point.setCoordinate1(qpoint[0])
        point.setCoordinate2(qpoint[1])
        deq = self.vptree.getAllInRange(point,maxDistance)
        return deq

        
    def __dealloc__(self):
        if self.vptree != NULL:
            del self.vptree
