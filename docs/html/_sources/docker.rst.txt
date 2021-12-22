
.. _NIFS-network:           https://www-net.nifs.ac.jp/network/index.htm
.. _development repository: https://github.com/nifs-software/nifs-retrieve


Quick Installation with Docker
===============================

.. note::
    For now, to use full functionalities, it is required to have the access to NIFS-LAN.
    Please see the `NIFS-network`_ page and get some way like SSL-VPN, and go down to the below section 
    after connecting to the NIFS-LAN.


docker image
------------

The easiest way to use thie package without downloding and installing
any other dependencies is to use `docker <https://docs.docker.com>`_ image.
How to download and install docker is available at https://docs.docker.com/get-docker/.
The built docker image for NIFS-retrieve is available at `here <https://hub.docker.com/r/koyom/nifs-retrieve>`_.

After installing the docker, the following command allows the user to create and enter the container:

.. prompt:: bash $

    docker run --rm -it  koyom/nifs-retrieve

Afterwards, if starting up the python (or iPython) interpreter, users can load :obj:`nifs.retrieve` module.


Jupyter notebook using docker image
-----------------------------------
For users who want to try this package, touching example notebooks is the best way.
To try to use this, plase download the source files from our `development repository`_.
If you have installed the git, using the following command is also available:

.. prompt:: bash $

    git clone https://github.com/nifs-software/nifs-retrieve

Once you have the source files, locate the foloder containing ``docker-compose.yml`` file,
and run:

.. prompt:: bash $

    docker-compose up -d

Then, the jupyter server has automatically started in the container, and the user has now access
to http://localhost:8888/ in your browser, where some example notebooks are available.

.. figure:: _static/images/nifs_retrieve_docker.gif
    :align: center
    
    **Caption** connecting to jupyter notebook after running the docker container

The following command terminates the docker proccess:

.. prompt:: bash $

    docker-compose down

