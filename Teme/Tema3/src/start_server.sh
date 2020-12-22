#!/bin/sh

while [ 1 ]; do
	nc -z sprc3_broker 1883 &> /dev/null \
	&& nc -z sprc3_influxdb 8086 &> /dev/null \
	&& break;

	sleep 1;
done

python3 -u server.py