#! /bin/bash

docker-compose -f stack.yml down
docker rmi eclipse-mosquitto:latest python:3.8 server:latest
