FROM pottava/openmpi:4.0

RUN apt-get install -y libomp-dev

ADD hybrid.c /home/mpiuser/
RUN mpicc -fopenmp hybrid.c -o hybrid

RUN mkdir .ssh && echo "StrictHostKeyChecking no" > .ssh/config
