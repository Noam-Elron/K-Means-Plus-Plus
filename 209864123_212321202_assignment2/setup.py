from setuptools import Extension, setup

module = Extension("mykmeanssp", sources=['kmeansmodule.c'])
setup(name='mykmeanssp',
     version='1.0',
     description='Python wrapper for KMeans in C',
     extra_compile_args=['-g'],  # Add debug symbols
     ext_modules=[module]
    )

