from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import os
ext_modules = [Extension("test",
                         ["vptree.pyx","VPTree.cpp","Distance.cpp","Point.cpp"],
                         include_dirs=['/usr/local/include'],
                         libraries=['Geographic'],
                         extra_compile_args=["-std=c++17"],
                         language='c++',
                     )]
setup(
  name = 'test',
  cmdclass = {'build_ext': build_ext},
  ext_modules = ext_modules
)
