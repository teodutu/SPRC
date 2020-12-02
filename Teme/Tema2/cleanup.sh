#! /bin/sh

docker-compose down -v
docker-compose rm

docker volume rm meteo-db
docker network rm network-api network-gui

docker rmi mysql:5.7 python:3.8 meteo-server:latest \
	ghcr.io/linuxserver/mysql-workbench:latest -f

rm -rf db/meteo-db/
