from datetime import datetime
from json import loads
import logging as log
from os import getenv
import re

from influxdb import InfluxDBClient
import paho.mqtt.client as mqtt


def _on_message(client, args, msg):
	if not re.match("^[^/]+/[^/]+$", msg.topic):
		return

	log.info(f'Received a message by topic [{msg.topic}]')

	location, station = msg.topic.split('/')
	data = loads(msg.payload)

	try:
		tstamp = datetime.strptime(data["timestamp"], '%Y-%m-%dT%H:%M:%S%z')
		log.info(f'Data timestamp is: {tstamp}')
	except:
		tstamp = datetime.now().strftime('%Y-%m-%dT%H:%M:%S%z')
		log.info('Data timestamp is NOW')

	json_data = [{
		"measurement": "iot_measurements",
		"tags": {
			"location": location,
			"station": station
		},
		"time": tstamp
	}]
	fields = {}

	for key, val in data.items():
		if type(val) != int and type(val) != float:
			continue

		fields[key] = val
		log.info(f'{f"{msg.topic}/{key}".replace("/", ".")} {val}')

	if fields:
		json_data[0]["fields"] = fields
		args.write_points(json_data)


def main():
	if getenv('DEBUG_DATA_FLOW') == "true":
		log_level = log.INFO
	else:
		log_level = log.ERROR

	log.basicConfig(
		format='%(asctime)s %(message)s',
		datefmt='%Y-%m-%d %H:%M:%S',
		level=log_level
	)

	db_cl = InfluxDBClient(host=getenv('DB'))
	db_cl.switch_database(getenv('DB_NAME'))

	mqtt_cl = mqtt.Client(userdata=db_cl)
	mqtt_cl.on_message = _on_message

	mqtt_cl.connect(getenv('BROKER'))
	mqtt_cl.subscribe('#')
	mqtt_cl.loop_forever()


if __name__ == "__main__":
	# TODO: iese cu codul 137. Fa-l sa iasa frumos
	main()
