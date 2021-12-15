from os import path as _path
from .anadata import *
from .database import *
from .rawdata import *

# parse the package version number
with open(_path.join(_path.dirname(__file__), "VERSION")) as _f:
    __version__ = _f.read().strip()
