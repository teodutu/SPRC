import argparse
import sys
from urllib.parse import urljoin

import paho.mqtt.client as mqtt


PUBLIC_ENDPOINT = "broker.hivemq.com"
QOS = 2


def _get_args():
	parser = argparse.ArgumentParser("Talk to other people via MQTT")
	parser.add_argument("-n", "--name", dest="name", type=str,
		nargs="?", default="TeoDutu", help="user's name")
	parser.add_argument("-t", "--topic", dest="topic", type=str, nargs="?",
		default="sprc/chat/#", help="the topic to which to subscribe")
	parser.add_argument("--id", dest="id", type=str, nargs="?",
		default="github.com/teodutu=valoare", help="the clientId")
	parser.add_argument("-p", "--public", dest="public", action="store_true",
		help="whether to broadcast to the public or local endpoint")

	return parser.parse_args()


def _on_connect(client, args, flags, rc):
	print(f"Connected with result code {rc}")
	client.subscribe(args.topic, QOS)


def _on_message(client, args, msg):
	if msg.topic == urljoin(args.topic, args.name):
		return

	print(f"[{msg.topic}]: {msg.payload.decode('utf-8').rstrip()}")


def create_client(args):
	client = mqtt.Client(args.id, clean_session=False, userdata=args)
	client.on_connect = _on_connect
	client.on_message = _on_message

	host_name = PUBLIC_ENDPOINT if args.public else "localhost"

	client.connect(host_name)
	client.loop_start()

	return client


def close_client(client):
	client.disconnect()
	client.loop_stop()


def main():
	args = _get_args()
	topic = urljoin(args.topic, args.name)

	cl = create_client(args)

	for line in sys.stdin:
		msg = line.rstrip()

		if "exit" == msg:
			break

		cl.publish(topic, msg, qos=QOS, retain=True)

	close_client(cl)


if __name__ == "__main__":
	main()
