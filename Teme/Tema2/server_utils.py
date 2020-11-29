from datetime import datetime
from flask import json, Response, request
from http import HTTPStatus
import MySQLdb


RESPONSE_CONTENT_TYPE = 'application/json'

ID = 'id'
COUNTRY_ID = 'idTara'
NAME = 'nume'
LAT = 'lat'
LONG = 'lon'
TIMESTAMP = 'timestamp'
CITY_ID = 'idOras'
VALUE = 'valoare'
FROM = 'from'
UNTIL = 'until'

COUNTRIES_DB = 'Tari'
CITIES_DB = 'Orase'
TEMPERATURES_DB = 'Temperaturi'


# TODO: vezi cum te conectezi asa cum vrea Hogea
# db = MySQLdb.connect(
# 	host='localhost',
# 	user='meteorology_user',
# 	passwd='meteorology_pass',
# 	db='meteorology_db'
# )


def _verify_req_body(request, fields):
	return all(field in request.json for field in fields)


def get_post(table, request, uniques, json_fields):
	if request.method == 'GET':
		# TODO: ia tot din tabela
		cities_json = [{ID: 2, COUNTRY_ID: 3, NAME: 'AiDPlm', LAT: 0.2, LONG: 0.69}]

		return Response(
			status=HTTPStatus.OK,
			response=json.dumps(cities_json),
			mimetype=RESPONSE_CONTENT_TYPE
		)
	else:
		if not _verify_req_body(request, json_fields):
			return Response(status=HTTPStatus.BAD_REQUEST)

		# TODO: verifica daca exista uniques in BD
		if False:
			return Response(status=HTTPStatus.CONFLICT)

		# TODO: updateaza in BD
		new_id = 99

		return Response(
			status=HTTPStatus.OK,
			response=json.dumps({ID: new_id}),
			mimetype=RESPONSE_CONTENT_TYPE
		)


def del_put(table, id, request, uniques, json_fields):
	try:
		id_int = int(id)
	except ValueError:
		return Response(status=HTTPStatus.NOT_FOUND)

	# TODO: verifica daca exista id_int in BD
	if id_int < 0:
		return Response(status=HTTPStatus.NOT_FOUND)

	if request.method == 'DELETE':
		# TODO: sterge din BD

		return Response(status=HTTPStatus.OK)
	else:
		if not _verify_req_body(request, [ID, COUNTRY_ID, NAME, LAT, LONG]):
			return Response(status=HTTPStatus.BAD_REQUEST)

		# TODO: updateaza in BD

		return Response(status=HTTPStatus.OK)


def get_temps_area(table, id, request):
	try:
		id_int = int(id)
	except ValueError:
		return Response(
			status=HTTPStatus.OK,
			response=json.dumps([]),
			mimetype=RESPONSE_CONTENT_TYPE
		)

	fromDate = datetime.strptime(
		request.args.get(FROM, default='0001-01-01', type=str),
		'%Y-%m-%d'
	)
	untilDate = datetime.strptime(
		request.args.get(UNTIL, default='9999-12-31', type=str),
		'%Y-%m-%d'
	)

	# TODO: ia din BD cu param de mai sus

	dummy_resp = [{ID: 2, VALUE: 1.11, TIMESTAMP: datetime.now().strftime('%Y-%m-%d')}]
	return Response(
		status=HTTPStatus.OK,
		response=json.dumps(dummy_resp),
		mimetype=RESPONSE_CONTENT_TYPE
	)
