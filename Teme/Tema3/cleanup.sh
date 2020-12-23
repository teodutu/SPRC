#! /bin/bash

docker stack rm sprc3
docker rmi -f python:latest grafana/grafana:latest server:latest \
	influxdb:latest eclipse-mosquitto:latest

docker volume prune
sudo rm -rf db/influxdb-storage
