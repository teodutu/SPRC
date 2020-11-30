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
DB_NAME = 'Meteorologie'


# TODO: vezi cum te conectezi asa cum vrea Hogea
db = MySQLdb.connect(
	host='localhost',
	user='root',
	passwd='root'
)
cr = db.cursor()


def _verify_req_body(request, fields):
	return all(field in request.json and type(request.json[field]) == t
		for field, t in fields.items())


def _get_field(request, field):
	tmp = request.json[field]

	if type(tmp) == str:
		return f"'{tmp}'"

	return tmp


def get_post(table, request, uniques, json_fields):
	if request.method == 'GET':
		cr.execute(f'SELECT * FROM {table}')

		keys = ["id"] + list(json_fields.keys())
		body = [{ keys[i]: record[i] for i in range(len(keys)) }
					for record in cr.fetchall()]

		return Response(
			status=HTTPStatus.OK,
			response=json.dumps(body),
			mimetype=RESPONSE_CONTENT_TYPE
		)
	else:
		if not _verify_req_body(request, json_fields):
			return Response(status=HTTPStatus.BAD_REQUEST)

		# verifica daca exista uniques in BD
		where_cond = 'and'.join([f'{uniq} = {_get_field(request, uniq)}'
			for uniq in uniques])
		cr.execute(f'SELECT {ID} FROM {table} WHERE {where_cond}')

		if cr.rowcount:
			return Response(status=HTTPStatus.CONFLICT)

		# updateaza in BD
		insert_body = ('(0, '
			+ ', '.join([f'{_get_field(request, field)}'
				for field in json_fields.keys()])
			+ ')')
		cr.execute(f'INSERT INTO {table} VALUES {insert_body}')

		cr.execute(f'SELECT {ID} FROM {table} WHERE {where_cond}')
		return Response(
			status=HTTPStatus.CREATED,
			response=json.dumps({ID: cr.fetchone()[0]}),
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
		if not _verify_req_body(request, json_fields):
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
