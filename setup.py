import os
import sys
import shutil

from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext




ext_modules = [Extension("vptree",
                         ["vptree.pyx","Point.cpp","Distance.cpp","VPTree.cpp"],
                         include_dirs=['/usr/local/include/GeographicLib'],
                         extra_compile_args=["-O3","-std=c++17"],
                         extra_link_args=["-std=c++17"],
                         libraries=['Geographic'],
                         language='c++',
                     )]
setup(
  name = 'vptree',
  cmdclass = {'build_ext': build_ext},
  ext_modules = ext_modules
)
