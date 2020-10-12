import argparse
import json
import requests
from os import path

from task2 import send_task2_request


TASK3_ENDPOINT = "https://sprc.dfilip.xyz/lab1/task3"


def _parse_args():
	parser = argparse.ArgumentParser(
		f"Send student's name to endpoint {TASK3_ENDPOINT}")
	parser.add_argument("-n", "--name", dest="student_name", type=str,
		nargs="?", default="Teodor-Stefan Dutu", help="student's name")
	parser.add_argument("--verbose", "-v", dest="verbose", action="store_true",
		help="print the message received when establishing a session")

	return parser.parse_args()


def _send_task3_get_request(session):
	endpoint = path.join(TASK3_ENDPOINT, "check")

	response = session.get(endpoint)
	if not response.ok:
		response.raise_for_status()

	return response.json()


def get_task3_proof(name, verbose=False):
	with requests.Session() as session:
		login_endpoint = path.join(TASK3_ENDPOINT, "login")
		response = send_task2_request(name, session=session,
			endpoint=login_endpoint)

		if verbose:
			print(response.text)

		return _send_task3_get_request(session)


if __name__ == "__main__":
	args = _parse_args()
	data = get_task3_proof(args.student_name, args.verbose)

	print(json.dumps(data, indent=4))
