#! /bin/bash

docker stack rm sprc3

docker rmi -f python:latest server:latest
sleep 20
docker image prune

docker volume prune
rm -rf ${SPRC_DVP}/iot_data_influxdb-storage
