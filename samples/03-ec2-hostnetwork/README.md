# MPI program execution on EC2 via Docker

## Create an AMI including OpenMPI and Docker

### Request a spot instance

```
# Ubuntu Server 18.04 LTS (HVM), SSD Volume Type
$ cat << EOF > config.json
{
  "ImageId": "ami-07ad4b1c3af1ea214",
  "InstanceType": "t2.small",
  "NetworkInterfaces": [
    {
        "DeviceIndex": 0,
        "SubnetId": "subnet-xxxxxxxx",
        "Groups": [ "sg-xxxxxxxx" ],
        "AssociatePublicIpAddress": true
    }
  ],
  "KeyName": "xxxxx"
}
EOF
$ aws ec2 request-spot-instances --spot-price "0.5" --instance-count 1 \
    --type "one-time" --launch-specification file://config.json
```

### Install dependencies

```
$ ssh -i xxxxx.pem ubuntu@xx.xxx.xx.x
# Install docker
# @see https://docs.docker.com/install/linux/docker-ce/ubuntu/#set-up-the-repository
ubuntu@ip-10-0-0-10:~# sudo usermod -aG docker ubuntu
ubuntu@ip-10-0-0-10:~# sudo systemctl disable docker
# Install OpenMPI
# @see https://github.com/pottava/docker-openmpi/blob/master/versions/4.0/Dockerfile
ubuntu@ip-10-0-0-10:~# exit
```

### Create an AMI from the instance

```
$ aws ec2 create-image --name "openmpi-v4.0.0" --description "OpenMPI v4.0.0" \
    --instance-id "i-xxxxxxxxxxxxxxxxx"
```

## Run instances

### Create a placement group for lower latency

```
$ aws ec2 create-placement-group --group-name "mpi-test" --strategy "cluster"
```

### Run instances

Request spot instances with c5.18xlarge ([Intel Xeon Platinum 8124M CPU@3.00GHz](https://en.wikichip.org/wiki/intel/xeon_platinum/8124m))

```
$ aws ec2 run-instances --image-id "ami-xxxxxxxxxxxxxxxxx" \
    --instance-type "c5.18xlarge" --cpu-options "CoreCount=36,ThreadsPerCore=2" --count 2 \
    --placement "GroupName=mpi-test" --ebs-optimized \
    --subnet-id "subnet-xxxxxxxx" --security-group-ids "sg-xxxxxxxx" \
    --associate-public-ip-address --key-name "xxxxx"
```

Or request a spot fleet

```
$ cat << EOF > config.json
{
  "AllocationStrategy": "lowestPrice",
  "SpotPrice": "2.0",
  "TargetCapacity": 2,
  "OnDemandTargetCapacity": 0,
  "IamFleetRole": "arn:aws:iam::xxxxxxxxxxxx:role/aws-service-role/spotfleet.amazonaws.com/AWSServiceRoleForEC2SpotFleet",
  "LaunchSpecifications": [
      {
          "ImageId": "ami-xxxxxxxxxxxxxxxxx",
          "InstanceType": "c5.18xlarge",
          "Placement": {"GroupName": "mpi-test"},
          "EbsOptimized": true,
          "NetworkInterfaces": [
              {
                  "DeviceIndex": 0,
                  "SubnetId": "subnet-xxxxxxxx",
                  "Groups": [ "sg-xxxxxxxx" ],
                  "AssociatePublicIpAddress": true
              }
          ],
          "KeyName": "xxxxx"
      } 
  ]
}
EOF
$ aws ec2 request-spot-fleet --spot-fleet-request-config file://config.json
```

### Setup MPI environment

Execute the following commands on each instances

```
$ ssh -i xxxxx.pem ubuntu@<instance-a-public-ip>

ubuntu@ip-10-0-0-10:~$ cat << EOF > ~/.ssh/config
StrictHostKeyChecking no
Host node01
    HostName <instance-a-private-ip>
    User ubuntu
    IdentityFile tokyo.pem
Host node02
    HostName <instance-b-private-ip>
    User ubuntu
    IdentityFile tokyo.pem
EOF
ubuntu@ip-10-0-0-10:~$ chmod 600 ~/.ssh/config
ubuntu@ip-10-0-0-10:~$ cat << EOF > xxxxx.pem
-----BEGIN RSA PRIVATE KEY-----
..
-----END RSA PRIVATE KEY-----
EOF
ubuntu@ip-10-0-0-10:~$ chmod 400 xxxxx.pem
ubuntu@ip-10-0-0-10:~$ ssh node01 hostname
ubuntu@ip-10-0-0-10:~$ ssh node02 hostname
```

### Run a samle MPI program

```
ubuntu@ip-10-0-0-10:~$ lscpu
ubuntu@ip-10-0-0-10:~$ cat << EOF > hello.c
#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int  rank, size, len;
    char name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    MPI_Get_processor_name( name, &len );
    name[len] = '\0';
    printf( "Hello, world, I am %d of %d from %s\n", rank, size, name );
    MPI_Barrier( MPI_COMM_WORLD );
    MPI_Finalize();
}
EOF
ubuntu@ip-10-0-0-10:~$ mpicc hello.c -o hello
ubuntu@ip-10-0-0-10:~$ mpiexec -np 2 ./hello
ubuntu@ip-10-0-0-10:~$ cat << EOF > hostfile
node01 slots=2
node02 slots=2
EOF
ubuntu@ip-10-0-0-10:~$ mpiexec -np 4 --hostfile hostfile ./hello
```

## Run the program via Docker

Execute the following commands on each instances

You can run a master node as a Docker container, as well.  
Worker nodes are still EC2 hosts though.

```
ubuntu@ip-10-0-0-10:~$ sudo systemctl start docker
ubuntu@ip-10-0-0-10:~$ docker run --rm -it \
    --net host -u mpiuser \
    -v $(pwd):/home/mpiuser \
    -v $HOME/.ssh:/home/mpiuser/.ssh \
    pottava/openmpi:4.0 bash
$ mpirun -np 2 ./hello
$ mpirun -np 4 --hostfile hostfile --mca btl_tcp_if_exclude docker0 ./hello
$ exit
```

Note: ttps://www.open-mpi.org/faq/?category=tcp#tcp-selection

## Run all nodes as Docker containers

### Build a docker image

```
ubuntu@ip-10-0-0-10:~$ cat << EOF > hybrid.c
#include <stdio.h>
#include "mpi.h"
#include <omp.h>

int main(int argc, char *argv[]) {
  int numprocs, rank, namelen;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int iam = 0, np = 1;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name(processor_name, &namelen);

  #pragma omp parallel default(shared) private(iam, np)
  {
    np = omp_get_num_threads();
    iam = omp_get_thread_num();
    printf("Hello from thread %d out of %d from process %d out of %d on %s\n",
           iam, np, rank, numprocs, processor_name);
  }
  MPI_Finalize();
}
EOF
ubuntu@ip-10-0-0-10:~$ cat << EOF > Dockerfile
FROM pottava/openmpi:4.0

RUN apt-get install -y libomp-dev

ADD xxxxx.pem /home/mpiuser/
RUN chown mpiuser /home/mpiuser/xxxxx.pem

ADD hostfile /home/mpiuser/
ADD hybrid.c /home/mpiuser/
RUN mpicc -fopenmp hybrid.c -o hybrid
EOF
ubuntu@ip-10-0-0-10:~$ docker build -t openmpi/samples:03-ec2 .
```

### Run the hybrid parallel program

Run it on a single node

```
ubuntu@ip-10-0-0-10:~$ docker run --rm -u mpiuser openmpi/samples:03-ec2 \
    mpirun -np 2 -x OMP_NUM_THREADS=36 ./hybrid
```

Run it on multiple nodes

```
ubuntu@ip-10-0-0-10:~$ docker run --rm \
    --net host -u mpiuser \
    -v $HOME/.ssh:/home/mpiuser/.ssh \
    openmpi/samples:03-ec2 \
    mpirun -np 2 --hostfile hostfile --mca btl_tcp_if_exclude docker0 \
    docker run --rm -u mpiuser openmpi/samples:03-ec2 \
    mpirun -np 2 -x OMP_NUM_THREADS=36 ./hybrid
```
