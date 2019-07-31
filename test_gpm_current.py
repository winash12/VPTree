import itertools
import time
import math
import h5py
from netCDF4 import Dataset
import sys
import wradlib as wrl
import datetime as dt


import numpy as np


from vptree import PyVPTree




def main():


    sr_pars = {"trmm": {
        "zt": 402500.,  # orbital height of TRMM (post boost)   APPROXIMATION!
        "dr": 250.,     # gate spacing of TRMM
    }, "gpm": {
        "zt": 407000.,  # orbital height of GPM                 APPROXIMATION!
        "dr": 125.,      # gate spacing of GPM
    }}
    
    bw_sr = 0.71                  # SR beam width
    platf = "gpm"                 # SR platform/product: one out of ["gpm", "trmm"]
    zt = sr_pars[platf]["zt"]     # SR orbit height (meters)
    
    dr_sr = sr_pars[platf]["dr"]  # SR gate length (meters)
    


    #gpm_file = wrl.util.get_wradlib_data_file('2A.GPM.Ku.V7-20170308.20180430-S132807-E150039.023692.V05A.HDF5')
    gpm_file = "2A.GPM.Ku.V7-20170308.20180430-S132807-E150039.023692.V05A.HDF5"
    sr_data = read_gpm(gpm_file,None)

    nscan_sr = sr_data['nscan']
    
    nray_sr = sr_data['nray']
    ngate_sr = sr_data['nbin']
    
    refl = sr_data['refl']
#    refl_filled = refl.filled(np.nan)

    reflNZ = refl[np.nonzero(refl)]

    #np.set_printoptions(threshold='nan')
    sr_lon = sr_data['lon']
    sr_lat = sr_data['lat']

    #sr_lon = sr_lon[...,np.newaxis]
    #sr_lat = sr_lat[...,np.newaxis]

    #sr_lon,refl_filled = np.broadcast_arrays(sr_lon,refl_filled)
    #sr_lat,refl_filled = np.broadcast_arrays(sr_lat,refl_filled)
    #print(sr_lon.shape,sr_lat.shape)
    reflp = np.zeros((ngate_sr,nray_sr,nscan_sr))
    reflC = np.transpose(refl,(2,1,0))
    reflp[reflC > 0]=1
    refls = np.sum(reflp,axis=0)
    sr_latT = sr_lat.T
    sr_lonT = sr_lon.T
    print(refls.shape,sr_lonT.shape)

    minLat = sr_latT[refls > 0].min()
    maxLat = sr_latT[refls > 0].max()
    minLon = sr_lonT[refls > 0].min()
    maxLon = sr_lonT[refls > 0].max()
    print(minLat,maxLat,minLon,maxLon)

    modMinLat = np.mod(minLat,0.05)
    modMaxLat = np.mod(maxLat,0.05)
    #print(modMinLat,modMaxLat)
    minL = minLat - modMinLat
    maxL = maxLat - modMaxLat
    modMinLon = np.mod(minLon,0.05)
    modMaxLon = np.mod(maxLon,0.05)
    #print(modMinLon,modMaxLon)
    minLo = minLon - modMinLon
    maxLo = maxLon - modMaxLon
    print(minLat,maxLat)

    latGrid = np.arange(minLat,maxLat,0.05)
    lonGrid = np.arange(minLo,maxLo,0.05)
    print(latGrid.shape,lonGrid.shape)

    gridLon,gridLat = np.meshgrid(lonGrid,latGrid)
    grid_points = np.c_[gridLon.ravel(),gridLat.ravel()]


    refl = np.transpose(refl,(2,0,1))
    i = 0
    for radar_element in refl:
        radar_data = radar_element[np.nonzero(radar_element)]
        lat_surface = sr_lat[np.nonzero(radar_element)]
        lon_surface = sr_lon[np.nonzero(radar_element)]
        print(lon_surface.shape,i)
        i +=1

        lon_surface = lon_surface.data
        print(hasattr(lon_surface,'mask'))
        lat_surface = lat_surface.data
        print(hasattr(lat_surface,'mask'))
        points = np.c_[lon_surface,lat_surface]
        points = np.array(points,dtype=np.float64)
        if points.size > 0:
            pvptree = PyVPTree()
            pvptree.initializeGreatCircleDistance()
            pvptree.buildPointsVector(points)
            sys.exit()
    total = time.time() - t0
    print(total)

def read_gpm(filename, bbox=None):
    """Reads GPM files for matching with GR

    Parameters
    ----------
    filename : string
        path of the GPM file
    bbox : dict
        dictionary with bounding box coordinates (lon, lat),
        defaults to None

    Returns
    -------
    gpm_data : dict
        dictionary of gpm data

    Examples
    --------
    See :ref:`/notebooks/match3d/wradlib_match_workflow.ipynb`.
    """
    pr_data = Dataset(filename, mode="r")
    lon = pr_data['NS'].variables['Longitude']
    lat = pr_data['NS'].variables['Latitude']

    if bbox is not None:
        poly = [[bbox['left'], bbox['bottom']],
                [bbox['left'], bbox['top']],
                [bbox['right'], bbox['top']],
                [bbox['right'], bbox['bottom']],
                [bbox['left'], bbox['bottom']]]
        mask = get_clip_mask(np.dstack((lon[:], lat[:])), poly)
    else:
        mask = np.ones_like(lon, dtype=bool, subok=False)

    mask = np.nonzero(np.count_nonzero(mask, axis=1))

    lon = lon[mask]
    lat = lat[mask]

    year = pr_data['NS']['ScanTime'].variables['Year'][mask]
    month = pr_data['NS']['ScanTime'].variables['Month'][mask]
    dayofmonth = pr_data['NS']['ScanTime'].variables['DayOfMonth'][mask]
    # dayofyear = pr_data['NS']['ScanTime'].variables['DayOfYear'][mask]
    hour = pr_data['NS']['ScanTime'].variables['Hour'][mask]
    minute = pr_data['NS']['ScanTime'].variables['Minute'][mask]
    second = pr_data['NS']['ScanTime'].variables['Second'][mask]
    # secondofday = pr_data['NS']['ScanTime'].variables['SecondOfDay'][mask]
    millisecond = pr_data['NS']['ScanTime'].variables['MilliSecond'][mask]
    date_array = zip(year, month, dayofmonth,
                     hour, minute, second,
                     millisecond.astype(np.int32) * 1000)
    pr_time = np.array(
        [dt.datetime(d[0], d[1], d[2], d[3], d[4], d[5], d[6]) for d in
         date_array])

    sfc = pr_data['NS']['PRE'].variables['landSurfaceType'][mask]
    pflag = pr_data['NS']['PRE'].variables['flagPrecip'][mask]

    # bbflag = pr_data['NS']['CSF'].variables['flagBB'][mask]
    zbb = pr_data['NS']['CSF'].variables['heightBB'][mask]
    # print(zbb.dtype)
    bbwidth = pr_data['NS']['CSF'].variables['widthBB'][mask]
    qbb = pr_data['NS']['CSF'].variables['qualityBB'][mask]
    qtype = pr_data['NS']['CSF'].variables['qualityTypePrecip'][mask]
    ptype = pr_data['NS']['CSF'].variables['typePrecip'][mask]

    quality = pr_data['NS']['scanStatus'].variables['dataQuality'][mask]
    refl = pr_data['NS']['SLV'].variables['zFactorCorrected'][mask]
    # print(pr_data['NS']['SLV'].variables['zFactorCorrected'])

    zenith = pr_data['NS']['PRE'].variables['localZenithAngle'][mask]

    pr_data.close()

    # Check for bad data
    if max(quality) != 0:
        raise ValueError('GPM contains Bad Data')

    pflag = pflag.astype(np.int8)

    # Determine the dimensions
    ndim = refl.ndim
    if ndim != 3:
        raise ValueError('GPM Dimensions do not match! '
                         'Needed 3, given {0}'.format(ndim))

    tmp = refl.shape
    nscan = tmp[0]
    nray = tmp[1]
    nbin = tmp[2]

    # Reverse direction along the beam
    refl = np.flip(refl, axis=-1)

    # Change pflag=1 to pflag=2 to be consistent with 'Rain certain' in TRMM
    pflag[pflag == 1] = 2

    # Simplify the precipitation types
    ptype = (ptype / 1e7).astype(np.int16)

    # Simplify the surface types
    imiss = (sfc == -9999)
    sfc = (sfc / 1e2).astype(np.int16) + 1
    sfc[imiss] = 0

    # Set a quality indicator for the BB and precip type data
    # TODO: Why is the `quality` variable overwritten?

    quality = np.zeros((nscan, nray), dtype=np.uint8)

    i1 = ((qbb == 0) | (qbb == 1)) & (qtype == 1)
    quality[i1] = 1

    i2 = ((qbb > 1) | (qtype > 2))
    quality[i2] = 2

    gpm_data = {}
    gpm_data.update({'nscan': nscan, 'nray': nray, 'nbin': nbin,
                     'date': pr_time, 'lon': lon, 'lat': lat,
                     'pflag': pflag, 'ptype': ptype, 'zbb': zbb,
                     'bbwidth': bbwidth, 'sfc': sfc, 'quality': quality,
                     'refl': refl, 'zenith': zenith})

    return gpm_data
    
main()
