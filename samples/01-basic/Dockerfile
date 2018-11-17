FROM pottava/openmpi:4.0

ADD hello.c /home/mpiuser/
RUN mpicc hello.c -o hello

RUN mkdir .ssh && echo "StrictHostKeyChecking no" > .ssh/config
