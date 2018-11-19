# MPI program execution on Kubernetes

## Build & push a docker image

Create a docker repository

```
$ aws configure
$ export AWS_DEFAULT_REGION=us-east-1
$ aws ecr create-repository --repository-name openmpi/samples
```

Build a docker image

```
$ cd samples/05-kubernetes
$ aws_account=$(aws sts get-caller-identity --query "Account" --output text)
$ img_tag="${aws_account}.dkr.ecr.${AWS_DEFAULT_REGION}.amazonaws.com/openmpi/samples:04-k8s"
$ docker build -t "${img_tag}" .
```

Push the image

```
$ $(aws ecr get-login --no-include-email)
$ docker push "${img_tag}"
```

## Create a deamon-set on a k8s cluster

Run the image as SSH servers which have OpenMPI inside

```
$ img_tag=${img_tag//\//\\\//}
$ sed -e "s/<image-name>/${img_tag}/" daemonset-template.yaml > daemonset.yaml
$ kubectl apply -f daemonset.yaml
$ kubectl get ds -o wide --watch
```

## Execute the job on the k8s cluster

### Run a hybrid parallel program with host network

```
$ ssh -i xxxxx.pem ubuntu@<instance-a-public-ip>

ubuntu@ip-10-0-0-10:~$ cat << EOF > $HOME/.ssh/config
StrictHostKeyChecking no
Host node01
    HostName <instance-a-private-ip>
    User mpiuser
    Port 2222
Host node02
    HostName <instance-b-private-ip>
    User mpiuser
    Port 2222
Host node03
    HostName <instance-c-private-ip>
    User mpiuser
    Port 2222
EOF
ubuntu@ip-10-0-0-10:~$ chmod 400 $HOME/.ssh/config
ubuntu@ip-10-0-0-10:~$ cat << EOF > hostfile
node01
node02
node03
EOF
ubuntu@ip-10-0-0-10:~$ ssh node01 hostname
ubuntu@ip-10-0-0-10:~$ aws_account=$(aws sts get-caller-identity --query "Account" --output text)
ubuntu@ip-10-0-0-10:~$ AWS_DEFAULT_REGION=us-east-1
ubuntu@ip-10-0-0-10:~$ docker run --rm -it \
    --net host -u mpiuser \
    -v $(pwd):/home/mpiuser \
    -v $HOME/.ssh:/home/mpiuser/.ssh \
    "${aws_account}.dkr.ecr.${AWS_DEFAULT_REGION}.amazonaws.com/openmpi/samples:04-k8s" \
    mpirun -np 3 --hostfile hostfile --mca btl_tcp_if_exclude docker0 \
    bash -c "export OMP_NUM_THREADS=4 && ./hybrid"
```

### Run a hybrid parallel program with kubernetes network

TODO

## Delete the deamon-set from the k8s cluster

```
$ kubectl delete ds openmpi-sample-05
```
