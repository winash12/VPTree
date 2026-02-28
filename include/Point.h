#ifndef POINT_H
#define POINT_H

struct alignas(16) Point {
    double lat;    // 8 bytes
    double lon;    // 8 bytes
    float dbz;     // 4 bytes
    int is_query;  // 4 bytes (0: GPM Data, 1: Grid Query)

    // Constructor A: For Grid Queries (Brodzik target)
    Point(double lt, double ln) 
        : lat(lt), lon(ln), dbz(0.0f), is_query(1) {} // REMOVED alt(0.0f)

    // Constructor B: For GPM Observations (The data in the tree)
    Point(double lt, double ln, float val) 
        : lat(lt), lon(ln), dbz(val), is_query(0) {}

    // Default constructor for pre-allocation
    Point() : lat(0.0), lon(0.0), dbz(0.0f), is_query(1) {}
};

#endif
