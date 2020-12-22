#! /bin/bash

docker-compose -f stack.yml down

docker rmi eclipse-mosquitto:latest python:latest server:latest \
            influxdb:latest grafana/grafana:latest

docker volume rm influxdb-storage
rm -rf db/

# TODO: vezi de ce mai apar volume:
docker volume prune -f
