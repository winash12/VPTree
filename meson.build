project('gpm_eytzinger_engine', 'cpp', 'cython',
  version : '0.1',
  default_options : [
    'cpp_std=c++17', 
    'optimization=3', 
    'buildtype=release'
  ])

# --- 1. Core Deps ---
py = import('python').find_installation(pure: false)
py_dep = py.dependency()
omp_dep = dependency('openmp', language: 'cpp')
geog_dep = dependency('geographiclib')

# --- 2. Dynamic NumPy Path (As String) ---
numpy_inc_path = run_command(py, [
  '-c', 'import numpy; print(numpy.get_include())'
], check: true).stdout().strip()

# --- 3. Source Mapping ---
sources = [
  'gpm_engine.pyx',
  'vptree.pyx',
  'VPTree.cpp'
]

# --- 4. The Extension Module ---
py.extension_module(
  'gpm_engine',
  sources: sources,
  # We add the NumPy path to cpp_args directly to avoid the absolute path error
  cpp_args: [
    '-march=native',
    '-mavx2',
    '-ffast-math',
    '-fopenmp',
    '-I' + numpy_inc_path, # Manually add NumPy C headers
    '-D_GLIBCXX_ASSERTIONS=1'
  ],
  # Standard local include for your .h files
  include_directories: include_directories('include'),
  dependencies: [py_dep, omp_dep, geog_dep],
  cython_args: [
    '--cplus',
    '-I' + meson.current_source_dir() + '/include'
  ],
  override_options: ['cython_language=cpp'],
  link_args: ['-fopenmp'],
  install: true
)