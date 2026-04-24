from libcpp.vector cimport vector
from libcpp.pair cimport pair
from libcpp.memory cimport shared_ptr
from libc.stdint cimport uint64_t, uint32_t

# --- 1. The Raw C++ Definitions (The Headers) ---

cdef extern from "Point.h" nogil:
    struct Point:
        double lat
        double lon
        float dbz      
        int is_query   
        Point() except +
        Point(double lt, double ln) except +
        Point(double lt, double ln, float val) except +

cdef extern from "VPTree.h" nogil:
    cdef cppclass VPTree:
        VPTree() except +
        vector[pair[double, shared_ptr[Point]]] getAllInRange(const Point* query, double maxDistance) nogil
        void initializeVPTreePoints(vector[shared_ptr[Point]]& points)

cdef extern from "gpm_eytz_engine.hpp" nogil:
    uint64_t encode_2d(double lat, double lon) nogil
    struct IndexRange:
        int i, j
    cdef cppclass VoxelIndex:
        VoxelIndex(const vector[uint64_t]& sorted_hashes,
                   const vector[Point]& sorted_coords,
                   const vector[float]& sorted_dbz) except +
        void get_safe_ranges(double lat_min, double lat_max, 
                             double lon_min, double lon_max,
                             vector[IndexRange]& results) const
        void collect_candidates(const vector[IndexRange]& ranges, 
                                vector[Point]& out_coords,
                                vector[float]& out_dbz) const

cdef extern from "Parallax.h" nogil:
    cdef cppclass ParallaxEngine:
        ParallaxEngine() except +
        void correct_bin(double& lat, double& lon, double heading, 
                        double height_m, int ray_idx) const

# --- 2. The Cython Class Definitions (The Owners) ---

cdef class PyVPTree:
    cdef VPTree* native_vptree
    cdef int num_procs

cdef class PyVoxelIndex:
    cdef VoxelIndex* index

