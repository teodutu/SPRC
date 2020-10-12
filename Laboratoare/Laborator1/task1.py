import argparse
import json
from os import path
import requests


TASK_ENDPOINT = "https://sprc.dfilip.xyz/lab1/task1"


def _parse_args():
	parser = argparse.ArgumentParser(
		f"Send student's name and group to endpoint {TASK_ENDPOINT}")
	parser.add_argument("-n", "--name", dest="student_name", type=str,
		nargs="?", default="Teodor-Stefan Dutu", help="student's name")
	parser.add_argument("-g", "--group" , dest="student_group", type=str,
		nargs="?", default="341C3", help="student's group")
	parser.add_argument("--check", dest="check", action="store_true",
		help="whether to check the task or not")

	return parser.parse_args()


def send_task1_request(name, group, check=False):
	if check:
		endpoint = path.join(TASK_ENDPOINT, "check")
	else:
		endpoint = TASK_ENDPOINT

	headers = {
		"secret2": "SPRCisBest"
	}
	data = {
		"secret": "SPRCisNice"
	}
	params = {
		"nume": name,
		"grupa": group,
	}
	params_str = "&".join(f"{key}={val}" for key, val in params.items())

	response = requests.post(endpoint, headers=headers, params=params_str,
		data=data)
	if not response.ok:
		response.raise_for_status()

	return response.json()


if __name__ == "__main__":
	args = _parse_args()
	data = send_task1_request(args.student_name, args.student_group, args.check)

	print(json.dumps(data, indent=4))
