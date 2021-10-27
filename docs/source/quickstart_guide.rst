****************
Quickstart Guide
****************

.. _list of analyzed data:     http://kaiseki-dev.lhd.nifs.ac.jp/documents/diagnostics/dataname.shtml
.. _search for registerd data: http://egdb.lhd.nifs.ac.jp/dataexplog/registered_data.html

======================
Retrieve analyzed data
======================

At first generate :py:class:`Anadata` instance using class method :py:meth:`Anadata.retrieve`:

.. code-block:: python

    from nifs.anadata import Anadata

    ana = Anadata.retrive("bolo", 149363, 1)

A diagnostics name is availabe from a `list of analyzed data`_
or the `search for registerd data`_ site.

=================
Retrieve raw data
=================