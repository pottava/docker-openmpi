# Hybrid parallel computing on docker

## 1. Build the image

```
$ cd samples/02-hybrid-parallel
$ docker build -t openmpi/samples:02-hybrid-parallel .
```

## 2. Start node containers

```
$ docker run --name 02-node01 -d --rm openmpi/samples:02-hybrid-parallel
$ docker run --name 02-node02 -d --rm openmpi/samples:02-hybrid-parallel
```

## 3. Execute a job

```
$ docker run --rm -it -u mpiuser \
    --link 02-node01:node01 --link 02-node02:node02 \
    openmpi/samples:02-hybrid-parallel \
    mpirun -np 2 --host node01,node02 -bind-to socket -x OMP_NUM_THREADS=4 ./hybrid
```

## 4. Clean up resources

```
$ docker rm -f $(docker ps -aq)
$ docker rmi openmpi/samples:02-hybrid-parallel
```
