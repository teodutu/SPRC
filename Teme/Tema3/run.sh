#! /bin/bash

mkdir -p ${SPRC_DVP}/influxdb-storage
mkdir -p ${SPRC_DVP}/grafana-storage

docker-compose -f stack.yml build
docker stack deploy -c stack.yml sprc3
