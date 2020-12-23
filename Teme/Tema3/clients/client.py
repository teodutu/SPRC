from json import dumps, load
from numpy import arange
from random import choice
from sys import stdin
from time import sleep

import paho.mqtt.client as mqtt


def _create_connection():
	client = mqtt.Client()
	client.connect("localhost")
	client.loop_start()

	return client


def _close_connection(client):
	client.disconnect()
	client.loop_stop()


def main():
	client = _create_connection()

	batts = list(range(90, 101))
	temps = list(range(20, 31))
	humids = list(range(30, 41))
	secs = list(arange(0.5, 1.6, 0.1))

	for station in ['A', 'B', 'C']:
		for _ in range(3):
			iot_data = {
				"BAT": choice(batts),
				"TEMP": choice(temps),
				"HUMID": choice(humids)
			}

			client.publish("asdf/" + station, dumps(iot_data))
			sleep(choice(secs))

	_close_connection(client)


if __name__ == "__main__":
	main()
