from libcpp.vector cimport vector
from libcpp.pair cimport pair
from libcpp.memory cimport shared_ptr
from libc.stdint cimport uint64_t, uint32_t

# 1. The Payload (Point.h)
cdef extern from "Point.h" nogil:
    struct Point:
        double lat
        double lon
        float dbz      # The GPM reflectivity for Cressman
        int is_query   # 0: Data, 1: Grid Query

        Point() except +
        Point(double lt, double ln) except +
        Point(double lt, double ln, float val) except +

# 2. The Search Engine (VPTree.h)
cdef extern from "VPTree.h" nogil:
    cdef cppclass Distance:
        pass

    cdef cppclass GreatCircleDistance(Distance):
        GreatCircleDistance() except +

    cdef cppclass VPTree:
        VPTree() except +
        # CRESSMAN SIGNATURE: This returns the (distance, Point) pairs needed for weighting
        vector[pair[double, shared_ptr[Point]]] getAllInRange(const Point* query, double maxDistance) nogil
        void initializeVPTreePoints(vector[shared_ptr[Point]]& points)
        void initializeDistance(Distance* distance)

# 3. The Clipping Engine (gpm_eytz_engine.hpp)
cdef extern from "gpm_eytz_engine.hpp" nogil:
    uint64_t  encode_2d(double lat, double lon) nogil

    struct IndexRange:
        int i
        int j

    cdef cppclass VoxelIndex:
        VoxelIndex(const vector[uint64_t]& sorted_hashes,
                   const vector[Point]& sorted_coords,
                   const vector[float]& sorted_dbz) except +
        
        IndexRange get_range(uint64_t low, uint64_t high) const
        void get_safe_ranges(double lat_min, double lat_max, 
                             double lon_min, double lon_max,
                             vector[IndexRange]& results) const
        void collect_candidates(const vector[IndexRange]& ranges, 
                                vector[Point]& out_coords,
                                vector[float]& out_dbz) const

# 4. The Science Engine (Parallax.h)
cdef extern from "Parallax.h" nogil:
    cdef cppclass ParallaxEngine:
        ParallaxEngine() except +
        void correct_bin(double& lat, double& lon, double heading, 
                        double height_m, int ray_idx) const


