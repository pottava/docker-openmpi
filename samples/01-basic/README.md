# Running a MPI program on docker

## 1. Build the image

```
$ cd samples/01-basic
$ docker build -t openmpi/samples:01-basic .
```

## 2. Start node containers

```
$ docker run --name 01-node01 -d --cpuset-cpus 0 openmpi/samples:01-basic
$ docker run --name 01-node02 -d --cpuset-cpus 1 openmpi/samples:01-basic
$ docker run --name 01-node03 -d --cpuset-cpus 2 openmpi/samples:01-basic
$ docker run --name 01-node04 -d --cpuset-cpus 3 openmpi/samples:01-basic
```

## 3. Execute a job

```
$ docker run --rm -it -u mpiuser \
    --link 01-node01:node01 --link 01-node02:node02 \
    --link 01-node03:node03 --link 01-node04:node04 \
    openmpi/samples:01-basic \
    mpirun -np 2 --host node01,node02,node03,node04 ./hello
```

## 4. Clean up resources

```
$ docker rm -f $(docker ps -aq)
$ docker rmi openmpi/samples:01-basic
```
