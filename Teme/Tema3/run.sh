#! /bin/bash

mkdir ${SPRC_DVP}/iot_data_influxdb-storage

docker-compose -f stack.yml build
docker stack deploy -c stack.yml sprc3
