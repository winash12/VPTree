#ifndef GPM_FINAL_ENGINE_HPP
#define GPM_FINAL_ENGINE_HPP

#include <vector>
#include <cstdint>
#include <algorithm>
#include <cstring>
#include <limits>
#include "eytzinger_array.h" // Pat Morin's Library

// 16-byte aligned for VP-Tree and SIMD efficiency
struct alignas(16) Point { double lat, lon; };
struct IndexRange { int i, j; };

/**
 * The Morton Encoder (Z-Order Curve)
 */
inline uint64_t spread(uint32_t x) {
  uint64_t res = x;
  res = (res | (res << 16)) & 0x0000FFFF0000FFFF;
  res = (res | (res << 8))  & 0x00FF00FF00FF00FF;
  res = (res | (res << 4))  & 0x0F0F0F0F0F0F0F0F;
  res = (res | (res << 2))  & 0x3333333333333333;
  res = (res | (res << 1))  & 0x5555555555555555;
  return res;
}

inline uint64_t encode_2d(double lat, double lon) {
  uint32_t lat_q = static_cast<uint32_t>(((lat + 90.0) / 180.0) * 4294967295.0);
  uint32_t lon_q = static_cast<uint32_t>(((lon + 180.0) / 360.0) * 4294967295.0);
  return (spread(lon_q) << 1) | spread(lat_q);
}

class VoxelIndex {
private:
  fbs::eytzinger_array_bfp<uint64_t, int, true>* eytz_searcher;
  std::vector<Point> coords; // Parallel Sorted Coords
  std::vector<float> dbz;    // Parallel Sorted Reflectivity
  int n;
  
public:
  VoxelIndex(const std::vector<uint64_t>& sorted_hashes,
             const std::vector<Point>& sorted_coords,
             const std::vector<float>& sorted_dbz) 
    : coords(sorted_coords), dbz(sorted_dbz), n((int)sorted_hashes.size()) {
    
    // Build Morin's high-speed rank-finder
    eytz_searcher = new fbs::eytzinger_array_bfp<uint64_t, int, true>(
                                                                      sorted_hashes.begin(), n
                                                                      );
  }

  ~VoxelIndex() { delete eytz_searcher; }
  
  inline IndexRange get_range(uint64_t low, uint64_t high) const {
    // Returns the RANK (sorted index) for O(1) range start
    return { eytz_searcher->search(low), 
             eytz_searcher->search(high + 1) };
  }
  
  void collect_candidates(const std::vector<IndexRange>& ranges, 
                          std::vector<Point>& out_coords,
                          std::vector<float>& out_dbz) const {
    size_t total = 0;
    for (const auto& r : ranges) if (r.i < r.j) total += (r.j - r.i);
    
    size_t start = out_coords.size();
    out_coords.resize(start + total);
    out_dbz.resize(start + total);
    
    for (const auto& r : ranges) {
      if (r.i >= r.j) continue;
      size_t count = r.j - r.i;
      // DMA-speed memory transfer
      std::memcpy(&out_coords[start], &coords[r.i], count * sizeof(Point));
      std::memcpy(&out_dbz[start], &dbz[r.i], count * sizeof(float));
      start += count;
    }
  }
  
  void get_safe_ranges(double lat_min, double lat_max, 
                       double lon_min, double lon_max,
                       std::vector<IndexRange>& results) const {
    results.clear();
    if (lon_max > 180.0) {
      results.push_back(get_range(encode_2d(lat_min, lon_min), encode_2d(lat_max, 180.0)));
      results.push_back(get_range(encode_2d(lat_min, -180.0), encode_2d(lat_max, lon_max - 360.0)));
    } else if (lon_min < -180.0) {
      results.push_back(get_range(encode_2d(lat_min, -180.0), encode_2d(lat_max, lon_max)));
      results.push_back(get_range(encode_2d(lat_min, lon_min + 360.0), encode_2d(lat_max, 180.0)));
    } else {
      results.push_back(get_range(encode_2d(lat_min, lon_min), encode_2d(lat_max, lon_max)));
    }
  }
};

#endif
