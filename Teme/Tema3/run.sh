#! /bin/bash

docker-compose -f stack.yml build
docker stack deploy -c stack.yml sprc3
