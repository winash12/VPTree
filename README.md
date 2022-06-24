To build this project all you have to do is 

1) a) Install Dependencies - GeographicLib C++ downloadable from https://sourceforge.net/projects/geographiclib/files/distrib/

b) Boost libraries - The C++ code needs Boost. Install them as per your OS flavor

c) Download https://github.com/QuantStack/xtensor and https://github.com/QuantStack/xtl as well

Build, test and install the above three then type

2) python3.8 setup.py build_ext --inplace

3) This will create a shared library file - vptree.cpython-36m-x86_64-linux-gnu.so

4) In order to use this in a python file you will need to update your PYTHON_PATH and LD_LIBRARY_PATH. Add the location of the shared library directory to both PYTHON_PATH and LD_LIBRARY_PATH
