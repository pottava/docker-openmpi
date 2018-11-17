# Running a MPI program on docker

## 1. Build the image

```
$ cd samples/01-basic
$ docker build -t openmpi/samples:01-basic .
```

## 2. Start node containers

```
$ docker run --name 01-node01 -d --rm openmpi/samples:01-basic
$ docker run --name 01-node02 -d --rm openmpi/samples:01-basic
```

## 3. Execute a job

```
$ docker run --rm -it -u mpiuser \
    --link 01-node01:node01 --link 01-node02:node02 \
    openmpi/samples:01-basic \
    mpirun -np 2 --host node01,node02 ./hello
```

## 4. Clean up resources

```
$ docker rm -f $(docker ps -aq)
$ docker rmi openmpi/samples:01-basic
```
