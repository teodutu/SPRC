#! /bin/sh

docker-compose down -v
docker-compose rm
docker system prune --volumes
docker rmi mysql:5.7 -f
docker rmi python:3.8 -f
docker rmi tema2_server:latest -f
rm -rf db/meteorology-db/
