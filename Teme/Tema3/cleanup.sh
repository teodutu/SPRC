#! /bin/bash

docker stack rm sprc3
docker rmi -f python:latest server:latest

rm -rf db/influxdb-storage
