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
	stations = ['A', 'B', 'C']

	while True:
		iot_data = {
			'BAT': choice(batts),
			'TEMP': choice(temps),
			'HUMID': choice(humids)
		}

		station = choice(stations)
		client.publish('UPB/' + station, dumps(iot_data))
		print(f'Station {station} published:\n{dumps(iot_data, indent=4)}\n')

		sleep(choice(secs))

	_close_connection(client)


if __name__ == "__main__":
	main()
