import numpy as np

from pyproj import Geod





def main():

    print(greatCircleDistance())
    
def greatCircleDistance():

    longitude1,latitude1 = (138.75323,-64.44619) 
    longitude2,latitude2 =  (138.85384,64.44619)
    g = Geod(a=180/np.pi,f=0)
    args = g.inv(longitude1,latitude1,longitude2,latitude2,radians=False)
    distance = args[2]
    distance = deg2km(distance)
    return distance

def deg2km(x):
    Re = 6371.
    km = (x * np.pi * Re)/180.
    return km


main()
