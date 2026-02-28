import numpy as np
cimport cython
from cython.parallel import prange
from libcpp.vector cimport vector
from libc.math cimport atan2, sin, cos, M_PI, tan

cdef class PyVPTree:
    cdef VPTree* native_vptree
    cdef int num_procs

# Define the C++ class for Cython
cdef extern from "Parallax.h" nogil:
    cdef cppclass ParallaxEngine:
        ParallaxEngine() except +
        void correct_bin(double& lat, double& lon, double heading, 
                        double height_m, int ray_idx) const

# Helper for Satellite Heading (Standard spherical heading is fine for short distances)
cdef inline double calculate_heading_nogil(double lat1, double lon1, double lat2, double lon2) nogil:
    cdef double dLon = (lon2 - lon1) * M_PI / 180.0
    cdef double l1 = lat1 * M_PI / 180.0
    cdef double l2 = lat2 * M_PI / 180.0
    cdef double y = sin(dLon) * cos(l2)
    cdef double x = cos(l1) * sin(l2) - sin(l1) * cos(l2) * cos(dLon)
    return (atan2(y, x) * 180.0 / M_PI + 360.0) % 360.0

@cython.boundscheck(False)
@cython.wraparound(False)
def apply_gpm_parallax(double[:, :, :] lats, 
                       double[:, :, :] lons, 
                       double bin_res_m = 125.0,
                       int num_threads = 4):
    
    cdef int s, r, b
    cdef int nscan = lats.shape[0]
    cdef int nray = lats.shape[1]
    cdef int nbin = lats.shape[2]
    
    cdef double heading, height_m

    cdef ParallaxEngine engine = ParallaxEngine()

    with nogil:
        for s in prange(nscan - 1, schedule='static', num_threads=num_threads):
            heading = calculate_heading_nogil(lats[s, 24, 0], lons[s, 24, 0], 
                                             lats[s+1, 24, 0], lons[s+1, 24, 0])
            for r in range(nray):
                for b in range(nbin):
                    height_m = (nbin - 1 - b) * bin_res_m
                    engine.correct_bin(lats[s, r, b], lons[s, r, b], 
                                      heading, height_m, r)

    @cython.boundscheck(False)
    @cython.wraparound(False)
    def generate_gpm_hashes(double[:, :, :] lats, double[:, :, :] lons):
        cdef int nscan = lats.shape[0]
        cdef int nray = lats.shape[1]
        cdef int nbin = lats.shape[2]
        cdef int s, r, b
        
        cdef uint64_t[:] hashes_view = np.empty(nscan * nray * nbin, dtype=np.uint64)
        cdef long long idx = 0
        
        with nogil:
            for s in prange(nscan, schedule='static'):
                for r in range(nray):
                    for b in range(nbin):
                        idx = <long long>s * nray * nbin + r * nbin + b
                        # Uses your gpm::encode_2d from gpm_eytz_engine.hpp
                        hashes_view[idx] = encode_2d(lats[s, r, b], lons[s, r, b])
        return np.asarray(hashes_view)

                    
cdef class PyVoxelIndex:
    cdef VoxelIndex* index

    def __cinit__(self, uint64_t[:] hashes, double[:, :] coords, float[:] dbz):
        cdef int n = hashes.shape[0]
        cdef int i
        cdef vector[uint64_t] v_hashes
        cdef vector[float] v_dbz
        cdef vector[Point] v_coords
        cdef Point p

        v_hashes.reserve(n)
        v_dbz.reserve(n)
        v_coords.reserve(n)

        for i in range(n):
            v_hashes.push_back(hashes[i])
            v_dbz.push_back(dbz[i])
            p.lat = coords[i, 0]
            p.lon = coords[i, 1]
            v_coords.push_back(p)

        self.index = new VoxelIndex(v_hashes, v_coords, v_dbz)

    def get_aoi_data(self, double lat_min, double lat_max, double lon_min, double lon_max):
        cdef vector[IndexRange] ranges
        cdef vector[Point] out_coords
        cdef vector[float] out_dbz
        
        with nogil:
            self.index.get_safe_ranges(lat_min, lat_max, lon_min, lon_max, ranges)
            self.index.collect_candidates(ranges, out_coords, out_dbz)
            
        cdef int count = out_coords.size()
        if count == 0:
            return np.empty((0, 2), dtype=np.float64), np.empty(0, dtype=np.float32)

        cdef double[:, :] res_coords = np.empty((count, 2), dtype=np.float64)
        cdef float[:] res_dbz = np.empty(count, dtype=np.float32)

        for i in range(count):
            res_coords[i, 0] = out_coords[i].lat
            res_coords[i, 1] = out_coords[i].lon
            res_dbz[i] = out_dbz[i]

        return np.asarray(res_coords), np.asarray(res_dbz)


    @cython.boundscheck(False)
    @cython.wraparound(False)
    def getCressmanMap(self, double[:, :] gridPoints, double maxDistance):
        cdef int n = gridPoints.shape[0]
        cdef int i, j
        cdef double R2 = maxDistance * maxDistance
        cdef PyVPTree self_ptr = <PyVPTree>self
        # 1. EXTRACT NATIVE POINTER (No GIL confusion)
        cdef VPTree* native_tree = self_ptr.native_vptree
    
        # 2. DECLARE ALL AT TOP
        cdef Point query_p 
        cdef double d2, w
        cdef Point* pt
        
        # WE USE 1-ELEMENT ARRAYS TO HIDE FROM CYTHON'S REDUCTION CHECKER
        cdef double s_w[1]
        cdef double s_wbz[1]
        
        cdef vector[vector[pair[double, shared_ptr[Point]]]] thread_results
        thread_results.resize(n)
        cdef double[:] grid_values = np.full(n, -9999.0, dtype=np.float64)

        with nogil:
            for i in prange(n, schedule='guided', num_threads=self.num_procs):
                # 3. INITIALIZE ARRAY ELEMENTS (This is now thread-private)
                s_w[0] = 0.0
                s_wbz[0] = 0.0
                
                query_p.lat = gridPoints[i, 0]
                query_p.lon = gridPoints[i, 1]
                query_p.dbz = 0.0
                query_p.is_query = 1
                
                thread_results[i] = native_tree.getAllInRange(&query_p, maxDistance)
                
                for j in range(thread_results[i].size()):
                    d2 = thread_results[i][j].first * thread_results[i][j].first
                    w = (R2 - d2) / (R2 + d2)
                    pt = thread_results[i][j].second.get()
                    
                    # 4. ACCUMULATE INTO ARRAY (Cython won't flag this as reduction)
                    s_wbz[0] += pt.dbz * w
                    s_w[0] += w
                
                # 5. READ FROM ARRAY (Perfectly legal)
                if s_w[0] > 0.0:
                    grid_values[i] = s_wbz[0] / s_w[0]

        return np.asarray(grid_values)

    
    def __dealloc__(self):
        if self.index != NULL:
            del self.index
