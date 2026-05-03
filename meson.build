project('gpm_eytzinger_engine', 'cpp', 'cython',
  version : '0.1',
  default_options : [
    'cpp_std=c++20', 
    'b_ndebug=if-release'
  ])

# --- 1. Tooling & Python Initialization ---
py_mod = import('python')
py = py_mod.find_installation(pure: false)
py_dep = py.dependency()

# --- 2. NumPy Detection ---
numpy_inc_path = run_command(py, [
  '-c', 'import numpy; print(numpy.get_include())'
], check: true).stdout().strip()

numpy_dep = declare_dependency(include_directories: include_directories(numpy_inc_path))

# --- 3. Core Dependencies ---
omp_dep = dependency('openmp', language: 'cpp')
geog_dep = dependency('geographiclib') 
pybind11_dep = dependency('pybind11')

# Manual include path for xtensor, xtl, and xtensor-python headers cloned in CI
xtensor_inc = include_directories('/usr/local/include')
xtensor_dep = declare_dependency(include_directories: xtensor_inc)
xt_py_dep = declare_dependency(include_directories: xtensor_inc)

# --- 4. Build Configuration & Includes ---
incdir = include_directories('include')

# Optimization flags
common_cpp_args = [
  '-march=native',   
  '-mavx2',          
  '-ffast-math',     
  '-D_GLIBCXX_ASSERTIONS=1'
]

# --- 5. Extension A: Modern xtensor/pybind11 Module ---
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
    numpy_dep 
  ],
  cpp_args: common_cpp_args,
  gnu_symbol_visibility: 'hidden',
  install: true
)

# --- 6. Extension B: Legacy Cython Module ---
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
    numpy_dep 
  ],
  cpp_args: common_cpp_args,
  cython_args: [
    '--cplus',
    '-I' + meson.current_source_dir() + '/include'
  ],
  override_options: ['cython_language=cpp'],
  install: true
)
