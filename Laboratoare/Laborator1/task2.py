import argparse
import json
import requests


TASK2_ENDPOINT = "https://sprc.dfilip.xyz/lab1/task2"


def _parse_args():
	parser = argparse.ArgumentParser(
		f"Send student's name to endpoint {TASK2_ENDPOINT}")
	parser.add_argument("-n", "--name", dest="student_name", type=str,
		nargs="?", default="Teodor-Stefan Dutu", help="student's name")

	return parser.parse_args()


def send_task2_request(name, session=requests, endpoint=TASK2_ENDPOINT):
	params = {
		"username": "sprc",
		"password": "admin",
		"nume": name
	}

	response = session.post(endpoint, json=params)
	if not response.ok:
		response.raise_for_status()

	return response


if __name__ == "__main__":
	args = _parse_args()
	response = send_task2_request(args.student_name)

	print(json.dumps(response.json(), indent=4))
