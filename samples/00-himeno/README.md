# Docker を使った姫野ベンチマーク

## 姫野ベンチマーク

http://accc.riken.jp/supercom/documents/himenobmt/

## C + MPI, static allocate version でのベンチマークを実施

http://i.riken.jp/supercom/documents/himenobmt/download/mpi-vpp/#itemid4514  
以下では次元数を `S 1 1 1` としていますが、実行環境に合わせて書き換えてみてください

```
$ docker run --rm -it pottava/openmpi:4.0 \
    bash -c "apt-get install -y unzip lhasa make >/dev/null \
    && wget --quiet http://i.riken.jp/wp-content/uploads/2015/07/cc_himenobmtxp_mpi.zip \
    && unzip -q cc_himenobmtxp_mpi.zip && lha xqw=/opt/himeno cc_himenobmtxp_mpi.lzh \
    && cd /opt/himeno && mv Makefile.sample Makefile \
    && chmod +x ./paramset.sh && ./paramset.sh S 1 1 1 && make >/dev/null 2>&1 \
    && mpirun --allow-run-as-root -np 1 /opt/himeno/bmt"
```
