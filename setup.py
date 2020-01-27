from setuptools import Extension, setup


setup(
    name="ketama",
    version="0.1.1",
    package_data={"": ["ketama.pyi"]},
    description="Python bindings for Ketama consistent hash library",
    long_description=open("README.rst").read(),
    url="https://github.com/mosquito/ketama-python",
    license="BSD License",
    ext_modules=[
        Extension(
            "ketama", ["ketama_python.c", "src/ketama.c", "src/md5.c"],
            extra_compile_args=["-std=gnu99"]
        )
    ],
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Intended Audience :: Science/Research",
        "Intended Audience :: System Administrators",
        "Intended Audience :: Telecommunications Industry",
        "License :: OSI Approved :: BSD License",
        "Natural Language :: English",
        "Operating System :: MacOS",
        "Operating System :: POSIX",
        "Programming Language :: C",
        "Programming Language :: Python :: 3 :: Only",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.4",
        "Programming Language :: Python :: 3.5",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: Implementation :: CPython",
        "Topic :: Communications",
        "Topic :: Internet",
        "Topic :: Software Development :: Libraries",
        "Topic :: System :: Clustering",
        "Topic :: System :: Distributed Computing",
    ],
)
