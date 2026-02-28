import numpy as np
import gpm_engine as ge

def process_3d_gpm_volume(lats_3d, lons_3d, dbz_3d, lat_min, lat_max, lon_min, lon_max):
    nscan, nray, nlevels = lats_3d.shape
    
    # 1. SCIENCE PHASE: Correct Parallax for the WHOLE volume first (In-place)
    # This is O(1) memory and uses all 8 threads.
    ge.apply_gpm_parallax(lats_3d, lons_3d, 125.0)

    final_3d_grid = []

    # 2. THE MEAT: Vertical Loop
    for level in range(nlevels):
        # Extract 2D slices for this height
        lats_2d = lats_3d[:, :, level]
        lons_2d = lons_3d[:, :, level]
        dbz_2d  = dbz_3d[:, :, level]

        # A. TOPOLOGY: Generate 64-bit Geohashes for this level
        # Uses your 'nogil' Morton encoder
        hashes = ge.generate_gpm_hashes(lats_2d, lons_2d)
        
        # B. INDEXING: Build the Eytzinger Mirror (VoxelIndex)
        # Sort first, then build Morin's high-speed searcher
        sort_idx = np.argsort(hashes)
        voxel_idx = ge.PyVoxelIndex(hashes[sort_idx], 
                                     lats_2d.ravel()[sort_idx], 
                                     lons_2d.ravel()[sort_idx], 
                                     dbz_2d.ravel()[sort_idx])

        # C. CLIPPING: Snatch Tamil Nadu AOI (AVX2 Speed)
        # This reduces 441,000 points (9k x 49) down to ~5,000 points
        clipped_coords, clipped_dbz = voxel_idx.get_aoi_data(lat_min, lat_max, lon_min, lon_max)

        if len(clipped_dbz) == 0:
            continue # Skip empty levels (e.g., above the storm)

        # D. SEARCH: Build VP-Tree for this specific level
        # We use the 3-attribute Point(lat, lon, dbz)
        tree = ge.PyVPTree(2, 8) 
        tree.initializeGreatCircleDistance()
        tree.buildPointsVector(clipped_coords[:, 0], clipped_coords[:, 1], clipped_dbz)

        # E. QUERY: Find neighbors for your grid
        # grid_points is your Tamil Nadu map (e.g., 0.05 deg resolution)
        results = tree.getNeighborsInRangeParallel(grid_points, 5000.0) # 5km radius
        
        # F. INTERPOLATION: Cressman Filter (The final rain value)
        level_map = apply_cressman_filter(results)
        final_3d_grid.append(level_map)
        
    return np.stack(final_3d_grid)
