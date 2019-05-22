import os
import sys
import shutil

from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
from Cython.Build import cythonize


try:
    if os.path.exists("vptree.cpp"):
        os.remove("vptree.cpp")
    if os.path.exists("vptree.cpython-36m-x86_64-linux-gnu.so"):
        os.remove("vptree.cpython-36m-x86_64-linux-gnu.so")
    if os.path.exists("build"):
        shutil.rmtree("build")
except OSError as e:
     print ("Error: %s - %s." % (e.filename, e.strerror))

os.environ['CFLAGS'] = '-O3 -Wall -std=c++17'
     
ext_modules = [Extension("vptree",
                         ["vptree.pyx","Point.cpp","Distance.cpp","VPTree.cpp"],
                         include_dirs=['/usr/local/include'],
                         #extra_compile_args=["-O3","-std=c++17"],
                         extra_link_args=["-std=c++17"],
                         libraries=['Geographic'],
                         language='c++',
                     )]
extensions = cythonize(ext_modules, language_level = "3")

setup(
  name = 'vptree',
  cmdclass = {'build_ext': build_ext},
  ext_modules = extensions
)
