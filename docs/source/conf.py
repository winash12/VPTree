import os
import sys
from datetime import datetime

# --- Path setup ---
# Points ONLY to the Shighra-Prakshepa root and build directory
sys.path.insert(0, os.path.abspath('../../'))
sys.path.insert(0, os.path.abspath('../../builddir'))

# --- Project information ---
project = 'Śīghra-Prakṣepa'
copyright = f'{datetime.now().year}, Aswin Dinakar'
author = 'Aswin Dinakar'
release = '1.0'

# --- General configuration ---
extensions = [
    'sphinx.ext.autodoc',     # For the Cython/Python bridge
    'sphinx.ext.napoleon',    # For scientific docstrings
    'sphinx.ext.viewcode',    # Links to source code
    'sphinx_rtd_theme',       # Professional sidebar theme
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']
language = 'en'

# --- Options for HTML output ---
html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']

# Settings for NumPy-style docstrings
napoleon_google_docstring = False
napoleon_numpy_docstring = True



