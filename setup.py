from setuptools import Extension, setup


setup(
    name='ketama',
    version='0.1.0',
    package_data={
        '': ['ketama.pyi'],
    },
    ext_modules=[
        Extension(
            "ketama", [
                "ketama_python.c",
                "src/ketama.c",
                "src/md5.c",
                "src/ketama.h",
                "src/md5.h"
            ],
        ),
    ],
)
