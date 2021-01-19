#! /bin/bash

docker stack rm sprc3

docker rmi -f python:latest server:latest
sleep 20
docker image prune

rm -rf db/influxdb-storage
rm -rf db/grafana-storage