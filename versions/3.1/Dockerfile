FROM debian:stretch-slim

RUN apt-get update \
    && apt-get install -y gcc ssh wget curl \
    && curl --location --silent --show-error --output /tini \
       https://github.com/krallin/tini/releases/download/v0.18.0/tini \
    && chmod +x /tini \
    && adduser --uid 1000 --home /home/mpiuser --shell /bin/bash \
       --disabled-password --gecos '' mpiuser \
    && passwd -d mpiuser \
    && apt-get install -y openssh-server \
    && mkdir -p /run/sshd /home/mpiuser/.ssh \
    && echo "StrictHostKeyChecking no" > /home/mpiuser/.ssh/config \
    && chown -R mpiuser /home/mpiuser \
    && sed -i s/#PermitRootLogin.*/PermitRootLogin\ no/ /etc/ssh/sshd_config \
    && sed -i s/#PubkeyAuthentication.*/PubkeyAuthentication\ no/ /etc/ssh/sshd_config \
    && sed -i s/.*UsePAM.*/UsePAM\ no/ /etc/ssh/sshd_config \
    && sed -i s/#PasswordAuthentication.*/PasswordAuthentication\ yes/ /etc/ssh/sshd_config \
    && sed -i s/#PermitEmptyPasswords.*/PermitEmptyPasswords\ yes/ /etc/ssh/sshd_config \
    && sed -i s/#ChallengeResponse.*/ChallengeResponseAuthentication\ no/ /etc/ssh/sshd_config \
    && sed -i s/#PermitUserEnvironment.*/PermitUserEnvironment\ yes/ /etc/ssh/sshd_config

ENV OPENMPI_VERSION=3.1.3 \
    LD_LIBRARY_PATH=/usr/local/lib \
    DEBCONF_NOWARNINGS=yes

RUN build_packages="build-essential" \
    && apt-get install -y ${build_packages} \
    && repo="https://www.open-mpi.org/software/ompi/v3.1/downloads" \
    && curl --location --silent --show-error --output openmpi.tar.gz \
      "${repo}/openmpi-${OPENMPI_VERSION}.tar.gz" \
    && echo '2cf91c5fb8f18acab239f6a8df4c0f0cd4da760b  openmpi.tar.gz' | sha1sum -c - \
    && tar xzf openmpi.tar.gz -C /tmp/ \
    && cd /tmp/openmpi-${OPENMPI_VERSION} \
    && ./configure --prefix=/usr/local \
    && make \
    && make install \
    && ldconfig \
    && cd / \
    && rm -rf /tmp/openmpi-${OPENMPI_VERSION} /openmpi.tar.gz \
    && apt-get purge -y --auto-remove ${build_packages}

WORKDIR /home/mpiuser

ENTRYPOINT ["/tini", "--"]
# do not detach (-D), log to stderr (-e)
CMD ["/usr/sbin/sshd", "-D", "-e"]
