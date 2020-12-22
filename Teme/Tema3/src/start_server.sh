#!/bin/sh

set -eu

while [ 1 ]; do
	nc -z mosquitto 1883 && nc -z influxdb 8086 && break;
	sleep 1;
done

python3 -u server.py