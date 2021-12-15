
============================
Downloading and Installation
============================
.. _NIFS-network:           https://www-net.nifs.ac.jp/network/index.html
.. _Retrieve-dbstore-SDK:   https://w3.lhd.nifs.ac.jp/LABCOM_Documents.htm
.. _LABCOM Download page:   https://w3.lhd.nifs.ac.jp/en/LABCOM_Download-e.htm
.. _libana2:                http://kaiseki-dev.lhd.nifs.ac.jp/software/libana/index-e_new.htm
.. _development repository: https://github.com/nifs-lhd/nifs

.. note::
    For now, to install the dependencies which are only accessible in the NIFS network,
    The users need to have the acceess there. Please see the `NIFS-network`_ page and get some way like SSL-VPN.

Requirements
============
The version python is assumed to be over 3.9+, and
the nifs-retrieve package requires the following packages:

* :obj:`numpy` >= 1.18
* :obj:`xarray`
* :obj:`psycopg2`
* :obj:`matplotlib`

:obj:`IPython` is recommended for the interactive use.

In addition, C++ external libraries: `Retrieve-dbstore-SDK`_, `libana2`_
have to be intalled in advance to build the nifs-retrieve package.
The procedures of downloading and installation of which are discribed below at first.



Retrieve-dbstore-SDK
~~~~~~~~~~~~~~~~~~~~~
`Retrieve-dbstore-SDK`_ is required for :ref:`rawdata<rawdata-module>` module to retrieve raw data from LABCOM server.
This SDK is available from `LABCOM Download page`_
or you can use ``wget`` command as follows:

.. prompt:: bash $
    
    cd ~/  # To download retrieve-dbstore-SDK to user home directory
    wget http://w3.lhd.nifs.ac.jp/LABCOM_Softwares/Retrieve/retrieve-dbstore-22.0.0.sdk.el8.tar.gz
    tar -xvzf retrieve-dbstore-22.0.0.sdk.el8.tar.gz
    mv SDK retrieve-dbstore-SDK  # rename

Finaly, configure the following environmental values in .bashrc:

.. prompt:: bash $

    echo "export RETRIEVE_PREFIX=$HOME/retrieve-dbstore-SDK" >> ~/.bashrc
    echo "export INDEXSERVERNAME=DasIndex.LHD.nifs.ac.jp/LHD" >> ~/.bashrc
    source ~/.bashrc


If you would rather download the SDK from website directly, you must choose CentOS8 SDK
whatever linux distribution your PC depends on.
And replace path names in the above code line with your own setting.



libana2
~~~~~~~~
`libana2`_ is a client library to use Kaiseki Server and experimental database.
:ref:`anadata<anadata-module>` module requires this library.
The source files are availabe from a git server. You can download it by ``git`` command:

.. prompt:: bash $

    cd ~/
    git clone http://kaiseki-dev.lhd.nifs.ac.jp:/git/kserver


Compile the libana2 library after moving into libana2 directory:

.. prompt:: bash $

    cd ~/kserver/libana2
    make install-head
    make install-lib

Afterwards, make sure that both include and lib directories are made in ~/kserver/libana2.

Configure the following environmental values in .bashrc:

.. prompt:: bash $

    echo "export LIBANA2_PREFIX=$HOME/kserver/libana2" >> ~/.bashrc
    echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LIBANA2_PREFIX/lib" >> ~/.bashrc
    source ~/.bashrc

Make symbolic links of .client.ini and .client2.ini files
which are stored in ~/kserver/kaiseki-client2 directory.
These files are written about the network information of Kiseki server.

.. prompt:: bash $

    ln -s ~/kserver/kaiseki-client2/.client.ini ~/.client.ini
    ln -s ~/kserver/kaiseki-client2/.client2.ini ~/.client2.ini

Or you can put these files into $HOME directory directly.


Installation
============
NIFS-retrieve package is available from our `development repository`_.
Once you have the source files, locate the foloder containing setup.py and run:

.. prompt:: bash $

    python setup.py install

If all the required dependencies are present,
this should start the NIFS-retrieve package compilation and installation process.

.. warning:: 

    Be sure to follow the above procedure of installing external C++ libraries
    before installing the NIFS package, otherwise, compilation errors might occur.


When developing NIFS-retrieve package, it is usually preferred that
the packages be installed in "develop" mode:

.. prompt:: bash $

    python setup.py develop

This will cause the original installation folder to be added to the site-package path.
Modifications to the code will therefore be visible to python next time the code is imported.


Testing
========
Examples and demos used throughout the NIFS-retrieve documentation are distributed with the source code in the ``demo`` folder or
``docs/source/examples`` folder.