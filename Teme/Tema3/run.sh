#! /bin/bash

if [ ! -d db/influxdb-storage ]; then
	mkdir db/influxdb-storage
fi

docker-compose -f stack.yml build
docker stack deploy -c stack.yml sprc3
