# Docker を使った MPI 計算

## サンプルイメージのビルド

```
$ cd samples/01-basic
$ docker build -t openmpi/samples:01-basic .
```

## ノードコンテナの起動

```
$ docker run --name 01-node01 -d --rm openmpi/samples:01-basic
$ docker run --name 01-node02 -d --rm openmpi/samples:01-basic
```

## 計算の実行

```
$ docker run --rm -it -u mpiuser \
    --link 01-node01:node01 --link 01-node02:node02 \
    openmpi/samples:01-basic \
    mpirun -np 2 --host node01,node02 ./hello
```

## コンテナの破棄

```
$ docker rm -f $(docker ps -aq)
$ docker rmi openmpi/samples:01-basic
```
