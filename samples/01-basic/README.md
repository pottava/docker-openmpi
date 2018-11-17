# Docker を使った MPI 計算

## サンプルイメージのビルド

```
$ cd samples/01-basic
$ docker build -t openmpi/samples:01-basic .
```

## ノードコンテナの起動

```
$ docker run --name node01 -d --rm openmpi/samples:01-basic
$ docker run --name node02 -d --rm openmpi/samples:01-basic
```

## 計算の実行

```
$ docker run --rm -it -u mpiuser \
    --link node01 --link node02 \
    openmpi/samples:01-basic \
    mpirun -np 2 --host node01,node02 ./hello
```
