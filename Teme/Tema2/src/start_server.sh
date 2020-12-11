#!/bin/sh

set -eu

while [ 1 ]; do
	nc -z mysql 3306 && break;
	sleep 1;
done

python3 server.py
