import wradlib as wrl
import pytest
import numpy as np
import os
import gpm_engine  

def test_gpm_legacy_integration():
    filename = "pythonTest/2A.GPM.Ku.V7-20170308.20180430-S132807-E150039.023692.V05A.HDF5"
    
    if not os.path.exists(filename):
        pytest.skip("Data file not found locally. Skipping I/O test.")
    
    gpm_data = wrl.io.gpm.read_gpm(filename)
    refl = gpm_data['refl']  
    lats = gpm_data['lat']   
    lons = gpm_data['lon']   

    nscan, nray, nbin = refl.shape
    lats_3d = np.repeat(lats[:, :, np.newaxis], nbin, axis=2).astype(np.float64)
    lons_3d = np.repeat(lons[:, :, np.newaxis], nbin, axis=2).astype(np.float64)

    # 1. Parallax (Modifies lats_3d and lons_3d in-place)
    gpm_engine.apply_parallax(lats_3d, lons_3d, bin_res_m=125.0)

    # 2. Build VP-Tree
    tree = gpm_engine.VPTree(n_procs=4)
    # Using 'build' to match your pybind11 .def("build", ...)
    tree.build(
        lats_3d.flatten(),
        lons_3d.flatten(),
        refl.flatten().astype(np.float32)
    )

    print("Test Passed: C++20 Engine successfully integrated with legacy workflow.")

if __name__ == "__main__":
    test_gpm_legacy_integration()
