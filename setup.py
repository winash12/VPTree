from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

ext_modules = [Extension("test",
                     ["vptree.pyx","VPTree.cpp","Distance.cpp","Point.cpp"],
                     language='c++',
                     )]

setup(
  name = 'test',
  cmdclass = {'build_ext': build_ext},
  ext_modules = ext_modules
)
