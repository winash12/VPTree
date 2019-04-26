# distutils: language = c++
# distutils: sources = Rectangle.cpp

cdef extern from "Point.h" namespace "tree":
    cdef cppclass Point:
         Point() except +
         double coordinate1, coordinate2
         double getCoordinate1()
         double getCoordinate2()

cdef extern from "Point.h" namespace "tree":
    cdef cppclass SphericalPoint:
         SphericalPoint() except +
         double coordinate1, coordinate2
         double getCoordinate1()
         double getCoordinate2()

