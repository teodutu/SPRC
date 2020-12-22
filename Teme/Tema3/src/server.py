from datetime import datetime
from json import loads
import re

import paho.mqtt.client as mqtt


# TODO: baga cu getenv
BROKER = 'mosquitto'


def _on_message(client, args, msg):
	if not re.match("^[^/]+/[^/]+$", msg.topic):
		return

	print(f'New message on topic {msg.topic}:')

	for key, val in loads(msg.payload).items():
		if type(val) != int and type(val) != float and key != "timestamp":
			continue

		if key == "timestamp":
			try:
				val = datetime.strptime(val, '%Y-%m-%dT%H:%M:%S%z')
			except ValueError:
				continue

		print(f'    {f"{msg.topic}/{key}".replace("/", ".")}: {val}')


def _(parameter_list):
	"""
	docstring
	"""
	pass


def main():
	client = mqtt.Client()
	client.on_message = _on_message
	client.on_disconnect =

	client.connect(BROKER)
	client.subscribe('#')
	client.loop_forever()


if __name__ == "__main__":
	# TODO: iese cu codul 137. Fa-l sa iasa frumos
	main()
