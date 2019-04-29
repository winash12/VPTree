#distutils : language = c++
#distutils : sources = VPTree.cpp

from vptree cimport VPTree

cdef class PyVPTree:
    cdef VPTree *vptree
    cdef Distance *gcd
    def __cinit__(self):
        self.vptree = new VPTree()


    def initializeGreatCircleDistance(self):
        self.gcd = new GreatCircleDistance()
        self.vptree.initializeDistance(self.gcd)
    
        
    def __dealloc__(self):
        if self.vptree != NULL:
            del self.vptree
