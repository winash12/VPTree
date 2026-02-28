// Example of using the GeographicLib::NearestNeighbor class.  Read lon/lat
// points for coast from coast.txt and lon/lat for vessels from vessels.txt.
// For each vessel, print to standard output: the index for the closest point
// on coast and the distance to it.

// This requires GeographicLib version 1.47 or later.

// Compile/link with, e.g.,
// g++ -I/usr/local/include -lGeographic -L/usr/local/bin -Wl,-rpath=/usr/local/lib -o coast coast.cpp

// Run time for 30000 coast points and 46217 vessels is 3 secs.

#include <iostream>
#include <exception>
#include <vector>
#include <fstream>

#include <GeographicLib/NearestNeighbor.hpp>
#include <GeographicLib/Geodesic.hpp>

using namespace std;
using namespace GeographicLib;

// A structure to hold a geographic coordinate.
struct pos {
  double _lat, _lon;
  pos(double lat = 0, double lon = 0) : _lat(lat), _lon(lon) {}
};

// A class to compute the distance between 2 positions.
class DistanceCalculator {
private:
  Geodesic _geod;
public:
  explicit DistanceCalculator(const Geodesic& geod) : _geod(geod) {}
  double operator() (const pos& a, const pos& b) const {
    double d;
    _geod.Inverse(a._lat, a._lon, b._lat, b._lon, d);
    if ( !(d >= 0) )
      // Catch illegal positions which result in d = NaN
      throw GeographicErr("distance doesn't satisfy d >= 0");
    return d;
  }
};

int main() {
  try {
    // Read in coast
    vector<pos> coast;
    double lat, lon;
    {
      ifstream is("coast.txt");
      if (!is.good())
        throw GeographicErr("coast.txt not readable");
      while (is >> lon >> lat)
        coast.push_back(pos(lat, lon));
      if (coast.size() == 0)
        throw GeographicErr("need at least one location");
    }

    // Define a distance function object
    DistanceCalculator distance(Geodesic::WGS84());

    // Create NearestNeighbor object
    NearestNeighbor<double, pos, DistanceCalculator>
      coastset(coast, distance);

    ifstream is("vessels.txt");
    double d;
    int count = 0;
    vector<int> k;
    while (is >> lon >> lat) {
      ++count;
      d = coastset.Search(coast, distance, pos(lat, lon), k);
      if (k.size() != 1)
          throw GeographicErr("unexpected number of results");
      cout << k[0] << " " << d << "\n";
    }
  }
  catch (const exception& e) {
    cerr << "Caught exception: " << e.what() << "\n";
    return 1;
  }
}
