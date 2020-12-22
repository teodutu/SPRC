from json import dumps, load
from sys import stdin

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

	for line in stdin:
		if "exit\n" == line:
			break

		topic = line.rstrip()
		with open("data.json") as f:
			msg = load(f)
		client.publish(topic, dumps(msg))

	_close_connection(client)


if __name__ == "__main__":
	main()
