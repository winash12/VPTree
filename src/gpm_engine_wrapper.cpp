#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <xtensor-python/pyarray.hpp>
#include <xtensor/views/xview.hpp>
#include <omp.h>
#include <cmath>

// Include your specialized headers
#include "VPTree.h"
#include "gpm_eytz_engine.hpp" 
#include "Parallax.h"

namespace py = pybind11;

// --- Helper for heading calculation (used in parallax) ---
inline double calculate_heading(double lat1, double lon1, double lat2, double lon2) {
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    double l1 = lat1 * M_PI / 180.0;
    double l2 = lat2 * M_PI / 180.0;
    double y = std::sin(dLon) * std::cos(l2);
    double x = std::cos(l1) * std::sin(l2) - std::sin(l1) * std::cos(l2) * std::cos(dLon);
    return std::fmod((std::atan2(y, x) * 180.0 / M_PI + 360.0), 360.0);
}

// ============================================================================
// 1. VP-Tree Python Interface
// ============================================================================
class PyVPTree {
public:
    std::shared_ptr<VPTree> native_vptree;
    int num_procs;

    PyVPTree(int n_procs = 4) : num_procs(n_procs) {
        native_vptree = std::make_shared<VPTree>();
    }

    void build(xt::pyarray<double>& lats, xt::pyarray<double>& lons, xt::pyarray<float>& dbz) {
        size_t n = lats.shape(0);
        std::vector<std::shared_ptr<Point>> pts;
        pts.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            pts.push_back(std::make_shared<Point>(lats(i), lons(i), dbz(i)));
        }
        native_vptree->initializeVPTreePoints(pts);
    }
};

// ============================================================================
// 2. VoxelIndex (Eytzinger) Python Interface
// ============================================================================
class PyVoxelIndex {
public:
    std::unique_ptr<VoxelIndex> index;

    PyVoxelIndex(xt::pyarray<uint64_t>& hashes, xt::pyarray<double>& coords, xt::pyarray<float>& dbz) {
        std::vector<uint64_t> v_hashes(hashes.begin(), hashes.end());
        std::vector<float> v_dbz(dbz.begin(), dbz.end());
        std::vector<Point> v_coords;
        v_coords.reserve(hashes.shape(0));
        for (size_t i = 0; i < hashes.shape(0); ++i) {
            v_coords.push_back({coords(i, 0), coords(i, 1)});
        }
        index = std::make_unique<VoxelIndex>(v_hashes, v_coords, v_dbz);
    }

    py::tuple query_aoi(double lat_min, double lat_max, double lon_min, double lon_max) {
        std::vector<IndexRange> ranges;
        std::vector<Point> out_coords;
        std::vector<float> out_dbz;
        {
            py::gil_scoped_release release;
            index->get_safe_ranges(lat_min, lat_max, lon_min, lon_max, ranges);
            index->collect_candidates(ranges, out_coords, out_dbz);
        }
        size_t count = out_coords.size();
        auto res_coords = xt::pyarray<double>::from_shape({count, 2});
        auto res_dbz = xt::pyarray<float>::from_shape({count});
        for (size_t i = 0; i < count; ++i) {
            res_coords(i, 0) = out_coords[i].lat;
            res_coords(i, 1) = out_coords[i].lon;
            res_dbz(i) = out_dbz[i];
        }
        return py::make_tuple(res_coords, res_dbz);
    }
};

// ============================================================================
// 3. Standalone GPM Functions
// ============================================================================

void apply_parallax(xt::pyarray<double>& lats, xt::pyarray<double>& lons, 
                    double bin_res_m = 125.0, int num_threads = 4) {
    
    // 1. Logistics: Get the dimensions of the NumPy array
    int nscan = lats.shape(0), nray = lats.shape(1), nbin = lats.shape(2);
    
    // 2. The Engine: Instantiate your class from Parallax.h
    ParallaxEngine engine; 

    {
        // 3. The Unlocker: Release Python's lock so we can use all CPU cores
        py::gil_scoped_release release;
        
        // 4. The Multiplier: Parallelize the work
        #pragma omp parallel for num_threads(num_threads)
        for (int s = 0; s < nscan - 1; ++s) {
            // Heading is calculated once per scan line
            double heading = calculate_heading(lats(s, 24, 0), lons(s, 24, 0), 
                                               lats(s+1, 24, 0), lons(s+1, 24, 0));
            for (int r = 0; r < nray; ++r) {
                for (int b = 0; b < nbin; ++b) {
                    double height_m = (nbin - 1 - b) * bin_res_m;
                    
                    // 5. The Execution: Call your Parallax.h logic
                    // This modifies the NumPy memory in-place via reference
                    engine.correct_bin(lats(s, r, b), lons(s, r, b), heading, height_m, r);
                }
            }
        }
    }
}

xt::pyarray<uint64_t> generate_hashes(xt::pyarray<double>& lats, xt::pyarray<double>& lons) {
    auto hashes = xt::pyarray<uint64_t>::from_shape(lats.shape());
    {
        py::gil_scoped_release release;
        #pragma omp parallel for collapse(2)
        for (int s = 0; s < (int)lats.shape(0); ++s) {
            for (int r = 0; r < (int)lats.shape(1); ++r) {
                for (int b = 0; b < (int)lats.shape(2); ++b) {
                    hashes(s, r, b) = encode_2d(lats(s, r, b), lons(s, r, b));
                }
            }
        }
    }
    return hashes;
}

xt::pyarray<double> cressman_map(PyVPTree& tree_wrapper, xt::pyarray<double>& gridPoints, double maxDistance) {
    size_t n = gridPoints.shape(0);
    double R2 = maxDistance * maxDistance;
    auto grid_values = xt::pyarray<double>::from_shape({n});
    grid_values.fill(-9999.0);

    {
        py::gil_scoped_release release;
        #pragma omp parallel for num_threads(tree_wrapper.num_procs) schedule(guided)
        for (int i = 0; i < (int)n; ++i) {
            Point query_p = {gridPoints(i, 0), gridPoints(i, 1)};
            auto neighbors = tree_wrapper.native_vptree->getAllInRange(&query_p, maxDistance);

            double sum_w = 0.0, sum_wbz = 0.0;
            for (const auto& res : neighbors) {
                double d2 = res.first * res.first;
                double w = (R2 - d2) / (R2 + d2);
                sum_wbz += res.second->dbz * w;
                sum_w += w;
            }
            if (sum_w > 0.0) grid_values(i) = sum_wbz / sum_w;
        }
    }
    return grid_values;
}

// ============================================================================
// 4. Module Definition
// ============================================================================
PYBIND11_MODULE(gpm_engine, m) {
    xt::import_numpy();

    py::class_<PyVPTree>(m, "VPTree")
        .def(py::init<int>(), py::arg("n_procs") = 4)
        .def("build", &PyVPTree::build);

    py::class_<PyVoxelIndex>(m, "VoxelIndex")
        .def(py::init<xt::pyarray<uint64_t>&, xt::pyarray<double>&, xt::pyarray<float>&>())
        .def("query_aoi", &PyVoxelIndex::query_aoi);

    m.def("apply_parallax", &apply_parallax);
    m.def("generate_hashes", &generate_hashes);
    m.def("cressman_map", &cressman_map);
}
