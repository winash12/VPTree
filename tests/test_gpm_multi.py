import wradlib as wrl
import pytest
import numpy as np
import os
import gpm_engine  # The actual compiled Cython module

def test_gpm_legacy_integration():
    """
    Test case using wradlib for I/O and Śīghra-Prakṣepa for engine logic.
    This demonstrates the end-to-end pipeline from NASA HDF5 to 3D VP-Tree.
    """
    
    # 1. SETUP PATHS
    # Ensure this points to the GPM file on your OptiPlex/ASUS
    filename = "pythonTest/2A.GPM.Ku.V7...HDF5" # Path on your OptiPlex
    
    if not os.path.exists(filename):
        # This tells the Cloud Runner: "It's okay that the file is gone, just move on."
        pytest.skip("Data file not found locally. Skipping I/O test.")
    else:
        # This only runs on your machine
        data = wrl.io.gpm.read_gpm(filename)
        assert 'refl' in data

    # Extract the core components
    refl = gpm_data['refl']  # 3D: (nscan, nray, nbin)
    lats = gpm_data['lat']   # 2D: (nscan, nray)
    lons = gpm_data['lon']   # 2D: (nscan, nray)

    print(f"Data shape: {refl.shape} (Scans, Rays, Bins)")

    # 3. GEOMETRIC PREPARATION
    # Your engine expects 3D coordinates for every single bin.
    # We broadcast the 2D Lat/Lon across the 176 vertical bins.
    nscan, nray, nbin = refl.shape
    lats_3d = np.repeat(lats[:, :, np.newaxis], nbin, axis=2)
    lons_3d = np.repeat(lons[:, :, np.newaxis], nbin, axis=2)

    # 4. INITIALIZE ŚĪGHRA-PRAKṢEPA ENGINE
    print("Initializing VP-Tree Engine...")
    tree = gpm_engine.PyVPTree(n_procs=4)

    # 5. PARALLAX CORRECTION
    # Correcting the 3D position based on the radar's look angle
    print("Running Parallax Correction...")
    gpm_engine.apply_gpm_parallax(
        lats_3d.astype(np.float64), 
        lons_3d.astype(np.float64), 
        bin_res_m=125.0
    )

    # 6. BUILD SPATIAL INDEX
    # Flatten the data for the build_tree function (double[:] expectations)
    print("Building VP-Tree Index...")
    tree.build_tree(
        lats_3d.flatten().astype(np.float64),
        lons_3d.flatten().astype(np.float64),
        refl.flatten().astype(np.float32)
    )

    print("Test Passed: Engine successfully ingested 9-year-old workflow data.")

if __name__ == "__main__":
    test_gpm_legacy_integration()
