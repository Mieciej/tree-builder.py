from setuptools import setup, Extension

setup(
    name="tree_builder",
    version="1.0",
    description="This is a demo package",
    # NOTE order of sources matters
    ext_modules=[Extension("tree_builder",
                           sources=["tree-builder/feature.cpp",
                                    "tree-builder/tree-buildermodule.cpp",],
                           include_dirs=["tree-builder"])]
)
