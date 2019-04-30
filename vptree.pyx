#distutils : language = c++
#distutils : sources = VPTree.cpp

from vptree cimport VPTree
from vptree cimport Distance
from vptree cimport Point

cimport numpy as np

cdef class PyVPTree:
    cdef VPTree *vptree
    cdef Distance *gcd
    cdef vector[shared_ptr[Point]]* points

    def __cinit__(self):
        self.vptree = new VPTree()
        self.points = new vector[shared_ptr[Point]]()
        
    def buildPointsVector(self,np.float64_t latitude,np.float64_t longitude):

        cdef shared_ptr[Point] point
        point = shared_ptr[Point](new SphericalPoint())
        #point.setCoordinate1(latitude)
        #point.setCoordinate2(longitude)
        self.points.push_back(point)
        
    cdef initializePoints(self):
        self.vptree.initializeVPTreePoints(self.points)
    
        
    def initializeGreatCircleDistance(self):
        self.gcd = new GreatCircleDistance()
        self.vptree.initializeDistance(self.gcd)
        
    def __dealloc__(self):
        if self.vptree != NULL:
            del self.vptree
