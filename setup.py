from setuptools import setup, Extension, find_packages
import subprocess
import numpy as np
import os
import os.path as path


# set paths to compilation includes and libraries
RETRIEVE_PREFIX = os.getenv("RETRIEVE_PREFIX")
if not RETRIEVE_PREFIX:
    raise ValueError("RETRIEVE_PREFIX environment variable must be set.")
RETRIEVE_INC_DIR = path.join(RETRIEVE_PREFIX, "include")
RETRIEVE_LIB_DIR = path.join(RETRIEVE_PREFIX, "lib64")
LIBANA2_PREFIX = os.getenv("LIBANA2_PREFIX")

if not LIBANA2_PREFIX:
    raise ValueError("LIBANA2_PREFIX environment variable must be set.")
LIBANA2_INC_DIR = path.join(LIBANA2_PREFIX, "include")
LIBANA2_LIB_DIR = path.join(LIBANA2_PREFIX, "lib")

try:
    POSTGRESQL_INC_DIR = subprocess.run(["pg_config", "--includedir"], stdout=subprocess.PIPE, text=True)
    POSTGRESQL_LIB_DIR = subprocess.run(["pg_config", "--libdir"], stdout=subprocess.PIPE, text=True)
    POSTGRESQL_INC_DIR = POSTGRESQL_INC_DIR.stdout.strip("\n")
    POSTGRESQL_LIB_DIR = POSTGRESQL_LIB_DIR.stdout.strip("\n")
except Exception:
    raise OSError("pg_config executable not found.")

# compilation includes and libraries
compilation_includes = {"_anadata": [np.get_include(), RETRIEVE_INC_DIR, POSTGRESQL_INC_DIR, LIBANA2_INC_DIR]}
compilation_library_dirs = {"_anadata": [RETRIEVE_LIB_DIR, POSTGRESQL_LIB_DIR, LIBANA2_LIB_DIR]}
compilation_libraries = {"_anadata": ["ana", "pq"], "_dbstore": ["dbstore"]}

source_paths = ["nifs"]
setup_path = path.dirname(__file__)

# build .cpp extensions
extensions = []
for package in source_paths:
    for root, dirs, files in os.walk(path.join(setup_path, package)):
        for file in files:
            if path.splitext(file)[1] == ".cpp":
                cpp_file = path.relpath(path.join(root, file), setup_path)
                module = path.splitext(cpp_file)[0].replace("/", ".")
                extensions.append(
                    Extension(
                        module,
                        [cpp_file],
                        include_dirs=compilation_includes.get(module.split(".")[-1], [RETRIEVE_INC_DIR]),
                        library_dirs=compilation_library_dirs.get(module.split(".")[-1], [RETRIEVE_LIB_DIR]),
                        libraries=compilation_libraries.get(module.split(".")[-1], ["retrieve"]),
                    )
                )

# parse the package version number
with open(path.join(setup_path, "nifs", "VERSION")) as version_file:
    version = version_file.read().strip()

# parse the prerequisite python packages
with open(path.join(setup_path, "requirements.txt"), "r") as f:
    install_requires = f.read().strip("\n")

setup(
    name="nifs",
    version=version,
    url="https://nifs-software.github.io/nifs-retrieve/",
    description="A package of modules for python to handle LHD experimetal data",
    license="BSD",
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Science/Research",
        "Intended Audience :: Education",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: BSD License",
        "Natural Language :: English",
        "Natural Language :: Japanese",
        "Operating System :: POSIX :: Linux",
        "Programming Language :: C++",
        "Programming Language :: Python :: 3",
        "Topic :: Database :: Retrieve LHD experimental data"
    ],
    install_requires=install_requires,
    packages=find_packages(),
    ext_modules=extensions,
)
