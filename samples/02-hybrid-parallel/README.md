# Docker を使ったハイブリッド並列計算

## サンプルイメージのビルド

```
$ cd samples/02-hybrid-parallel
$ docker build -t openmpi/samples:02-hybrid-parallel .
```

## ノードコンテナの起動

```
$ docker run --name 02-node01 -d --rm openmpi/samples:02-hybrid-parallel
$ docker run --name 02-node02 -d --rm openmpi/samples:02-hybrid-parallel
```

## 計算の実行

```
$ docker run --rm -it -u mpiuser \
    --link 02-node01:node01 --link 02-node02:node02 \
    openmpi/samples:02-hybrid-parallel \
    mpirun -np 2 --host node01,node02 -bind-to socket -x OMP_NUM_THREADS=4 ./hybrid
```

## コンテナの破棄

```
$ docker rm -f $(docker ps -aq)
$ docker rmi openmpi/samples:02-hybrid-parallel
```
