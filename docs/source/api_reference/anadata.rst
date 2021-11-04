

.. _libana2: http://kaiseki-dev.lhd.nifs.ac.jp/software/libana/index-e_new.htm

Anadata module
===============

This module can handle the analyzed data stored in Kaiseki server.
(retrieving, registoring, etc.)
Retrieving method is implemeted in C++ modules (`libana2`_), and retrieved dataset
is constitued by ``xarray.Dataset``.


.. autoclass:: nifs.anadata.AnaData
    :members:

.. autoclass:: nifs.anadata.NewAnaData
    :members:
