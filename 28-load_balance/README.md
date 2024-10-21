
# Setup for gRPC Load Balance test application

## Create networks
```bash
$ docker network create net1
$ docker network create net2
```

## Build container
```bash
$ docker build -t grpc-prod -f Dockerfile
```
## Setup DNS on hosts
```bash
$ sudo vi /etc/hosts
172.18.0.2 prod.local
172.19.0.2 prod.local
```
## Run 2 containers
```bash
$ docker run --rm -it -v .:/workspaces --network net1 -p 6001:6000 --name prod1 grpc-prod
$ docker run --rm -it -v .:/workspaces --network net2 -p 6002:6000 --name prod2 grpc-prod 
```
## Execute server on containers:
```bash
At prod1:
./server 6000

At prod2:
./server 6000
```

## Execute client on host:
```bash
Set policy:
$ ln -s policy_rr.json policy.json

Run app:
$ ./client prod.local:6000 10
```