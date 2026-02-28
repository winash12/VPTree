from libcpp.vector cimport vector
from libcpp.pair cimport pair
from libcpp.memory cimport shared_ptr

cdef extern from "Point.h" nogil:
    cdef cppclass Point:
        Point() except +
        double lat
        double lon
        float dbz
        int is_query
        Point(double lt, double ln) except +
        Point(double lt, double ln, float val) except +
        
cdef extern from "Distance.h" nogil:
    cdef cppclass Distance:
        # Note: Match the C++ signature exactly (const Point*)
        double calculateDistance(const Point* p1, const Point* p2) const

    cdef cppclass GreatCircleDistance(Distance):
        GreatCircleDistance() except +
        double calculate(const Point* p1, const Point* p2) const

cdef extern from "VPTree.h" nogil:
    cdef cppclass VPTree:
        VPTree() except +
        # Matches the thread_local version we just wrote
        vector[pair[double, shared_ptr[Point]]] getAllInRange(const Point* query, double maxDistance) nogil
        void initializeVPTreePoints(vector[shared_ptr[Point]]& points)
        void initializeDistance(Distance *distance)
