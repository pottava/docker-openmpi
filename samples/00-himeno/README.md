# Himeno benchmark with docker

## Introduction to the Himeno benchmark @riken.jp

http://accc.riken.jp/supercom/documents/himenobmt/

## Using `C + MPI, static allocate` version

http://i.riken.jp/supercom/documents/himenobmt/download/mpi-vpp/#itemid4514  
Although the following commands use `S 1 1 1` as its parameter, please modify that as you like.

```
$ docker run --rm -it pottava/openmpi:4.0 \
    bash -c "apt-get install -y unzip lhasa make >/dev/null \
    && wget --quiet http://i.riken.jp/wp-content/uploads/2015/07/cc_himenobmtxp_mpi.zip \
    && unzip -q cc_himenobmtxp_mpi.zip && lha xqw=/opt/himeno cc_himenobmtxp_mpi.lzh \
    && cd /opt/himeno && mv Makefile.sample Makefile \
    && chmod +x ./paramset.sh && ./paramset.sh S 1 1 1 && make >/dev/null 2>&1 \
    && su -c 'mpirun -np 1 /opt/himeno/bmt' mpiuser"
```
