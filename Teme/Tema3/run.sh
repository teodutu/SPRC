#! /bin/bash

mkdir -p db/influxdb-storage
mkdir -p db/grafana-storage

docker-compose -f stack.yml build
docker stack deploy -c stack.yml sprc3
