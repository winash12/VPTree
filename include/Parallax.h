#ifndef PARALLAX_H
#define PARALLAX_H

#include <GeographicLib/Geodesic.hpp>
#include <cmath>

class ParallaxEngine {
private:
  // Declare the Look-Up Table here so the functions can access it
  double tan_lut[49]; 
public:
  // No constructor needed now, or keep a default one
  ParallaxEngine() {
    static constexpr double beam_width = 0.71;
    static constexpr double deg2rad = M_PI / 180.0;
    for (int i = 0; i < 49; ++i) {
      tan_lut[i] = std::tan(std::abs(i - 24) * beam_width * deg2rad);
    }
  }

  // ORIGINAL METHOD: Uses std::tan() every single time (Safe but slower)
  inline void correct_bin(double& lat, double& lon, double heading, 
                          double height_m, int ray_idx) const {
    
    static const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84();
    static constexpr double beam_width = 0.71; 
    
    if (ray_idx == 24 || height_m <= 0.0) return; 
    
    double incidence_deg = std::abs(ray_idx - 24) * beam_width;
    double shift_az = (ray_idx < 24) ? (heading - 90.0) : (heading + 90.0);
    double shift_dist = height_m * std::tan(incidence_deg * M_PI / 180.0);
    
    double out_lat, out_lon;
    geod.Direct(lat, lon, shift_az, shift_dist, out_lat, out_lon);
    
    lat = out_lat;
    lon = out_lon;
  }

  // REFACTORED METHOD: Uses the LUT (The Fast "Śīghra" Way)
  inline void correct_bin_fast(double& lat, double& lon, double heading, 
                               double height_m, int ray_idx) const {
    
    if (ray_idx == 24 || height_m <= 0.0) return; 
    
    // LUT fetch instead of tan() calculation
    double shift_dist = height_m * tan_lut[ray_idx];
    
    // Azimuth logic
    double shift_az = (ray_idx < 24) ? (heading - 90.0) : (heading + 90.0);
    
    double out_lat, out_lon;
    GeographicLib::Geodesic::WGS84().Direct(lat, lon, shift_az, shift_dist, out_lat, out_lon);
    
    lat = out_lat;
    lon = out_lon;
  }
};

#endif
