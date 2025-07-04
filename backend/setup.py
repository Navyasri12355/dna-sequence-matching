from setuptools import setup, Extension

module = Extension("stringmatch", sources=["stringmatch.c"])

setup(
    name="stringmatch",
    version="1.0",
    description="String Matching Algorithms in C",
    ext_modules=[module]
)
