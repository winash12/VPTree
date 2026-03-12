import numpy as np
cimport numpy as np
cimport cython
from cython.parallel import prange
from libcpp.vector cimport vector
from libcpp.pair cimport pair
from libcpp.memory cimport shared_ptr
from libc.math cimport atan2, sin, cos, M_PI, tan
from libc.stdint cimport uint64_t
from libcpp.memory cimport shared_ptr, make_shared
# --- 1. Class Implementations (No 'cdef' variables here, they are in the .pxd) ---

cdef class PyVPTree:
    def __cinit__(self, int n_procs=4):
        self.native_vptree = new VPTree()
        self.num_procs = n_procs

    def build_tree(self, double[:] lats, double[:] lons, float[:] dbz):
        cdef vector[shared_ptr[Point]] pts
        cdef int n = lats.shape[0]
        pts.reserve(n)
        for i in range(n):
            pts.push_back(make_shared[Point](lats[i], lons[i], dbz[i]))
        self.native_vptree.initializeVPTreePoints(pts)

    def __dealloc__(self):
        if self.native_vptree != NULL:
            del self.native_vptree

cdef class PyVoxelIndex:
    def __cinit__(self, uint64_t[:] hashes, double[:, :] coords, float[:] dbz):
        cdef int n = hashes.shape[0]
        cdef vector[uint64_t] v_hashes
        v_hashes.reserve(n)
        cdef vector[Point] v_coords
        v_coords.reserve(n)
        cdef vector[float] v_dbz
        v_dbz.reserve(n)
        cdef Point p
        
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

        # 1. Pre-allocate the NumPy buffers (The "Python" side)
        cdef double[:, :] res_coords = np.empty((count, 2), dtype=np.float64)
        cdef float[:] res_dbz = np.empty(count, dtype=np.float32)

        # 2. Manual fast-copy from C++ to NumPy
        # (This is still very fast on an Optiplex 7050)
        for i in range(count):
            res_coords[i, 0] = out_coords[i].lat
            res_coords[i, 1] = out_coords[i].lon
            res_dbz[i] = out_dbz[i]

        # 3. Return the NumPy views
        return np.asarray(res_coords), np.asarray(res_dbz)

    def __dealloc__(self):
        if self.index != NULL:
            del self.index

# --- 2. Standalone Functions (Top Level - Left Aligned) ---

@cython.boundscheck(False)
@cython.wraparound(False)
def apply_gpm_parallax(double[:, :, :] lats, double[:, :, :] lons, double bin_res_m=125.0, int num_threads=4):
    cdef int s, r, b
    cdef int nscan = lats.shape[0], nray = lats.shape[1], nbin = lats.shape[2]
    cdef double heading, height_m
    cdef ParallaxEngine engine = ParallaxEngine()
    with nogil:
        for s in prange(nscan - 1, schedule='static', num_threads=num_threads):
            heading = calculate_heading_nogil(lats[s, 24, 0], lons[s, 24, 0], lats[s+1, 24, 0], lons[s+1, 24, 0])
            for r in range(nray):
                for b in range(nbin):
                    height_m = (nbin - 1 - b) * bin_res_m
                    engine.correct_bin(lats[s, r, b], lons[s, r, b], heading, height_m, r)

@cython.boundscheck(False)
@cython.wraparound(False)
def generate_gpm_hashes(double[:, :, :] lats, double[:, :, :] lons):
    # 1. DECLARE C-TYPES HERE
    cdef int s, r, b
    cdef int nscan = lats.shape[0]
    cdef int nray = lats.shape[1]
    cdef int nbin = lats.shape[2]
    
    # Pre-allocate output
    cdef uint64_t[:] hashes_view = np.empty(nscan * nray * nbin, dtype=np.uint64)
    cdef long long idx
    
    with nogil:
        # 2. Cython now knows 's', 'r', and 'b' are C integers
        for s in prange(nscan, schedule='static'):
            for r in range(nray):
                for b in range(nbin):
                    # Manual index calculation for the flat output array
                    idx = <long long>s * nray * nbin + r * nbin + b
                    
                    # CALL THE C++ ENGINE (This must be marked 'nogil' in .pxd)
                    hashes_view[idx] = encode_2d(lats[s, r, b], lons[s, r, b])
                    
    return np.asarray(hashes_view)
# --- 3. Parallel Interpolation (Moved inside PyVPTree or STANDALONE) ---

@cython.boundscheck(False)
@cython.wraparound(False)
def getCressmanMapParallel(PyVPTree tree_obj, double[:, :] gridPoints, double maxDistance):
    cdef int n = gridPoints.shape[0]
    cdef double R2 = maxDistance * maxDistance
    cdef double[:] grid_values = np.full(n, -9999.0, dtype=np.float64)
    cdef vector[vector[pair[double, shared_ptr[Point]]]] thread_results
    thread_results.resize(n)
    
    cdef Point query_p
    cdef double sum_w, sum_wbz, d2, w
    cdef Point* pt_ptr

    cdef double[:] all_sum_w = np.zeros(n, dtype=np.float64)
    cdef double[:] all_sum_wbz = np.zeros(n, dtype=np.float64)
    
    with nogil:
        for i in prange(n, schedule='guided', num_threads=tree_obj.num_procs):
            # 2. Use the 'i' index to store results directly in the arrays
            # This completely avoids the 'reduction' variable conflict
            
            query_p.lat = gridPoints[i, 0]
            query_p.lon = gridPoints[i, 1]
            
            thread_results[i] = tree_obj.native_vptree.getAllInRange(&query_p, maxDistance)
            
            for j in range(thread_results[i].size()):
                d2 = thread_results[i][j].first * thread_results[i][j].first
                w = (R2 - d2) / (R2 + d2)
                pt_ptr = thread_results[i][j].second.get()
                
                # NO REDUCTION VARIABLES HERE
                all_sum_wbz[i] += pt_ptr.dbz * w
                all_sum_w[i] += w
            
            # 3. Now it is 100% safe to read because it's a specific array index
            if all_sum_w[i] > 0.0:
                grid_values[i] = all_sum_wbz[i] / all_sum_w[i]
    return np.asarray(grid_values)

# --- 4. Utilities ---
cdef inline double calculate_heading_nogil(double lat1, double lon1, double lat2, double lon2) nogil:
    cdef double dLon = (lon2 - lon1) * M_PI / 180.0
    cdef double l1 = lat1 * M_PI / 180.0
    cdef double l2 = lat2 * M_PI / 180.0
    cdef double y = sin(dLon) * cos(l2)
    cdef double x = cos(l1) * sin(l2) - sin(l1) * cos(l2) * cos(dLon)
    return (atan2(y, x) * 180.0 / M_PI + 360.0) % 360.0
