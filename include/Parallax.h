#ifndef PARALLAX_H
#define PARALLAX_H

#include <GeographicLib/Geodesic.hpp>
#include <cmath>

class ParallaxEngine {
public:
    // No constructor needed now, or keep a default one
    ParallaxEngine() {}

    inline void correct_bin(double& lat, double& lon, double heading, 
                           double height_m, int ray_idx) const {
        
        // Use static members directly
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
};

#endif
