FROM debian:stretch-slim

ENV OPENMPI_VERSION=4.0.0 \
    LD_LIBRARY_PATH=/usr/local/lib

RUN apt-get update && apt-get install -y gcc ssh wget curl

RUN build_packages="build-essential" \
    && apt-get update \
    && apt-get install -y ${build_packages} \
    && repo="https://www.open-mpi.org/software/ompi/v4.0/downloads" \
    && curl --location --silent --show-error --output openmpi.tar.gz \
      "${repo}/openmpi-${OPENMPI_VERSION}.tar.gz" \
    && echo '951a6c18bd5117403455f5a1dd21e6e1157ef1d4  openmpi.tar.gz' | sha1sum -c - \
    && tar xzf openmpi.tar.gz -C /tmp/ \
    && cd /tmp/openmpi-${OPENMPI_VERSION} \
    && ./configure --prefix=/usr/local \
    && make \
    && make install \
    && ldconfig \
    && cd / \
    && rm -rf /tmp/openmpi-${OPENMPI_VERSION} /openmpi.tar.gz \
    && apt-get purge -y --auto-remove ${build_packages}
