from setuptools import setup, Extension
import os
setup(
    name="tree_builder",
    version="1.0",
    description="This is a demo package",
    # NOTE order of sources matters
    ext_modules=[Extension("tree_builder",
                           sources=["tree-builder/attribute.cpp",
                                    "tree-builder/tree.cpp",
                                    "tree-builder/utils.cpp",
                                    "tree-builder/tree-buildermodule.cpp",
                                    ],
                           include_dirs=[f"tree-builder"],
                           language="c++" )]
)
