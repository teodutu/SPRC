#! /bin/bash

docker stack rm sprc3

docker rmi -f python:latest server:latest
sleep 20
docker image prune

rm -rf ${SPRC_DVP}
