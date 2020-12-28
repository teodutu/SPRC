#! /bin/bash

docker stack rm sprc3

docker rmi -f python:latest server:latest
sleep 10
docker image prune

docker volume prune
