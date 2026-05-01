#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <xtensor-python/pyarray.hpp>
#include <memory>
#include <vector>
#include <omp.h>

// Your original headers
#include "include/VPTree.h"
#include "include/Point.h"
#include "include/Distance.h"
#include "include/GreatCircleDistance.h" 

namespace py = pybind11;

/**
 * VPTreeWrapper handles the interface between NumPy/Python and the 
 * C++ VPTree implementation. It manages memory for the distance 
 * metric and the root of the tree.
 */
class VPTreeWrapper {
private:
    std::shared_ptr<VPTree> root;
    std::unique_ptr<Distance> dist_calc;
    int num_procs;

public:
    // Constructor
    VPTreeWrapper(int n_cols, int n_procs = 4) : num_procs(n_procs) {
        root = std::make_shared<VPTree>();
    }

    // Sets up the specific distance metric logic
    void initializeGreatCircleDistance() {
        dist_calc = std::make_unique<GreatCircleDistance>();
        root->initializeDistance(dist_calc.get());
    }

    // Converts NumPy arrays to C++ Point vectors and builds the tree
    void buildPointsVector(xt::pyarray<double>& lat, 
                          xt::pyarray<double>& lon, 
                          xt::pyarray<float>& dbz) {
        
        size_t n = lat.shape(0);
        std::vector<std::shared_ptr<Point>> pts;
        pts.reserve(n);

        for (size_t i = 0; i < n; ++i) {
            // point.h: Point(double lat, double lon, float dbz)
            pts.push_back(std::make_shared<Point>(lat(i), lon(i), dbz(i)));
        }
        
        // Build the VP-Tree structure (O(N log N))
        root->initializeVPTreePoints(pts);
    }

    // Parallel search using OpenMP and the thread_local stacks in VPTree.cpp
    py::list getNeighborsInRangeParallel(xt::pyarray<double>& gridPoints, double maxDistance) {
        size_t n = gridPoints.shape(0);
        
        // Storage for results from each query point
        std::vector<std::vector<std::pair<double, std::shared_ptr<Point>>>> thread_results(n);

        {
            // Release the Global Interpreter Lock so threads can run in parallel
            py::gil_scoped_release release;
            
            #pragma omp parallel for num_threads(num_procs) schedule(guided)
            for (int i = 0; i < static_cast<int>(n); ++i) {
                Point p;
                p.lat = gridPoints(i, 0);
                p.lon = gridPoints(i, 1);
                
                // thread_local nodes_to_visit vector ensures this is thread-safe
                thread_results[i] = root->getAllInRange(&p, maxDistance);
            }
        }

        // Convert C++ results back to a Python list of lists
        py::list final_output;
        for (size_t i = 0; i < n; ++i) {
            py::list point_neighbors;
            for (const auto& res : thread_results[i]) {
                auto& pt_ptr = res.second;
                point_neighbors.append(py::make_tuple(
                    res.first,                             // Distance
                    py::make_tuple(pt_ptr->lat, pt_ptr->lon), // Coordinates
                    pt_ptr->dbz                            // Value
                ));
            }
            final_output.append(point_neighbors);
        }

        return final_output;
    }
};

// This block creates the actual Python module
PYBIND11_MODULE(shighra_vptree, m) {
    // Initialize xtensor's numpy support
    xt::import_numpy();

    m.doc() = "High-performance Vantage Point Tree implementation using xtensor and pybind11";

    // Export VPTreeWrapper to Python as "VPTree"
    py::class_<VPTreeWrapper>(m, "VPTree")
        .def(py::init<int, int>(), py::arg("n_cols"), py::arg("n_procs") = 4)
        .def("initializeGreatCircleDistance", &VPTreeWrapper::initializeGreatCircleDistance)
        .def("buildPointsVector", &VPTreeWrapper::buildPointsVector)
        .def("getNeighborsInRangeParallel", &VPTreeWrapper::getNeighborsInRangeParallel);
}
