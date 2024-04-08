from setuptools import setup, Extension

setup(
    name="spam",
    version="1.0",
    description="This is a demo package",
    ext_modules=[Extension("spam", sources=["spam/spammodule.c"])],
)