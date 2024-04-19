from setuptools import setup, Extension

setup(
    name="tree_builder",
    version="1.0",
    description="This is a demo package",
    ext_modules=[Extension("tree_builder", sources=["tree-builder/tree-buildermodule.cpp"])],
)
