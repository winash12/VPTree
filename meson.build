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
# We bypass include_directories() because Meson blocks absolute paths 
# pointing inside the source tree (which happens when using a .venv).
numpy_inc_path = run_command(py, [
  '-c', 'import numpy; print(numpy.get_include())'
], check: true).stdout().strip()

# Add the path directly to project arguments so Meson doesn't check it
add_project_arguments('-I' + numpy_inc_path, language: 'cpp')

# Create an empty dependency so the rest of the script remains valid
numpy_dep = declare_dependency()


# --- 3. Core Dependencies ---
omp_dep = dependency('openmp', language: 'cpp')
geog_dep = dependency('geographiclib') 
pybind11_dep = dependency('pybind11')

# FIX: We pass the absolute system path directly to the compiler args.
# This avoids the "Absolute path to a dir in the source tree" error.
add_project_arguments('-I/usr/local/include', language: 'cpp')

# We declare these as empty dependencies so the extension_module logic stays clean.
xtensor_dep = declare_dependency()
xt_py_dep = declare_dependency()

# --- 4. Build Configuration & Includes ---
# This uses a relative path, which Meson allows.
incdir = include_directories('include')

# Optimization flags for AVX2 and speed
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
