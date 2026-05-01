project('gpm_eytzinger_engine', 'cpp', 'cython',
  version : '0.1',
  default_options : [
    'cpp_std=c++20', 
    'b_ndebug=if-release'
  ])

# --- 1. Tooling & Python Initialization ---
# We find Python first so we can use it to probe for NumPy
py_mod = import('python')
py = py_mod.find_installation(pure: false)
py_dep = py.dependency()

# --- 2. NumPy Detection ---
# This executes a small python snippet to find the exact header location
numpy_inc_path = run_command(py, [
  '-c', 'import numpy; print(numpy.get_include())'
], check: true).stdout().strip()

# Create a dependency object that we can plug into our modules
numpy_dep = declare_dependency(include_directories: include_directories(numpy_inc_path))

# --- 3. Core Dependencies ---
omp_dep = dependency('openmp', language: 'cpp')
geog_dep = dependency('geographiclib') # For ParallaxEngine
pybind11_dep = dependency('pybind11')
xtensor_dep = dependency('xtensor')
xt_py_dep = declare_dependency(include_directories: include_directories('/usr/local/include'))

# --- 4. Build Configuration & Includes ---
incdir = include_directories('include')

# Processor-specific flags for your Optiplex 7050 (i7-7700)
common_cpp_args = [
  '-march=native',   
  '-mavx2',          
  '-ffast-math',     
  '-D_GLIBCXX_ASSERTIONS=1'
]

# --- 5. Extension A: Modern xtensor/pybind11 Module ---
# High-speed backend with zero-copy NumPy views
py.extension_module(
  'gpm_engine', 
  sources: [
    'src/gpm_engine_wrapper.cpp',
    'src/VPTree.cpp'
  ],
  include_directories: incdir,
  dependencies: [
    py_dep, 
    omp_dep, 
    geog_dep, 
    pybind11_dep, 
    xtensor_dep, 
    xt_py_dep,
    numpy_dep  # Crucial for xtensor-python
  ],
  cpp_args: common_cpp_args,
  gnu_symbol_visibility: 'hidden',
  install: true
)

# --- 6. Extension B: Legacy Cython Module ---
# Replicating the legacy setup for comparison or backward compatibility
py.extension_module(
  'vptree_cython',
  sources: [
    'cython/vptree.pyx',
    'src/VPTree.cpp'
  ],
  include_directories: incdir,
  dependencies: [
    py_dep, 
    omp_dep, 
    geog_dep,
    numpy_dep # Cython requires this to find "numpy/arrayobject.h"
  ],
  cpp_args: common_cpp_args,
  cython_args: [
    '--cplus',
    '-I' + meson.current_source_dir() + '/include'
  ],
  override_options: ['cython_language=cpp'],
  install: true
)