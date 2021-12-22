FROM python:3.9

# -----------------------------------------------------------------------------
#
# Retrieve-dbstore-SDK
# 
# -----------------------------------------------------------------------------
WORKDIR /opt
RUN wget http://w3.lhd.nifs.ac.jp/LABCOM_Softwares/Retrieve/retrieve-dbstore-22.0.0.sdk.el8.tar.gz &&\
    tar -xvzf retrieve-dbstore-22.0.0.sdk.el8.tar.gz &&\
    rm -rf retrieve-dbstore-22.0.0.sdk.el8.tar.gz

ENV RETRIEVE_PREFIX=/opt/SDK
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${RETRIEVE_PREFIX}/lib64
ENV INDEXSERVERNAME=DasIndex.LHD.nifs.ac.jp/LHD


# -----------------------------------------------------------------------------
#
# pycopg2
#
# -----------------------------------------------------------------------------
RUN python -m pip install --upgrade pip &&\
    pip install psycopg2


# -----------------------------------------------------------------------------
#
# Downgrade gcc/g++ library to 9
#
# -----------------------------------------------------------------------------
RUN apt-get update && apt-get install g++-9-multilib -y &&\
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 30 &&\
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 30


# -----------------------------------------------------------------------------
#
# libana2
#
# -----------------------------------------------------------------------------
WORKDIR /usr/local/src
RUN --mount=type=secret,id=bitbucket GIT_PASS=$(cat /run/secrets/bitbucket) &&\
    git clone https://munechika_koyo:${GIT_PASS}@bitbucket.org/emoto_masahiko/kserver.git &&\
    cd /usr/local/src/kserver/libana2 &&\
    make install-head && make install-lib

ENV LIBANA2_PREFIX=/usr/local/src/kserver/libana2
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${LIBANA2_PREFIX}/lib

RUN ln -s /usr/local/src/kserver/kaiseki-client2/.client.ini ~/.client.ini &&\
    ln -s /usr/local/src/kserver/kaiseki-client2/.client2.ini ~/.client2.ini


# -----------------------------------------------------------------------------
#
# nifs-retrieve
#
# -----------------------------------------------------------------------------
WORKDIR /usr/local/src
RUN git clone https://github.com/nifs-software/nifs-retrieve.git
WORKDIR /usr/local/src/nifs-retrieve
RUN pip install -r requirements.txt &&\
    pip install -e .


# -----------------------------------------------------------------------------
#
# Install Jupyter notebook
#
# -----------------------------------------------------------------------------
WORKDIR /home
RUN python -m pip install jupyter
CMD [ "bash" ]