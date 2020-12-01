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
	host='mysql',
	port=3306,
	user='meteo-admin',
	passwd='meteo',
	db=DB_NAME
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


def make_get_response(keys, records):
	body = [{keys[i]: record[i] for i in range(len(keys))}
		for record in records]

	return Response(
		status=HTTPStatus.OK,
		response=json.dumps(body),
		mimetype=RESPONSE_CONTENT_TYPE
	)


def get_post(table, request, uniques, json_fields):
	if request.method == 'GET':
		cr.execute(f'SELECT * FROM {table}')
		return make_get_response([ID] + list(json_fields.keys()), cr.fetchall())
	else:
		if not _verify_req_body(request, json_fields):
			return Response(status=HTTPStatus.BAD_REQUEST)

		insert_body = ('('
			+ ', '.join([f'{_get_field(request, field)}'
				for field in json_fields.keys()])
			+ ')')
		insert_cols = '(' + ', '.join(json_fields.keys()) + ')'

		try:
			cr.execute(
				f'INSERT INTO {table} {insert_cols} VALUES {insert_body}'
			)
			db.commit()
		except:
			return Response(status=HTTPStatus.CONFLICT)

		cr.execute('SELECT LAST_INSERT_ID()')
		return Response(
			status=HTTPStatus.CREATED,
			response=json.dumps({ID: cr.fetchone()[0]}),
			mimetype=RESPONSE_CONTENT_TYPE
		)


def del_put(table, id, request, json_fields):
	try:
		id_int = int(id)
	except ValueError:
		return Response(status=HTTPStatus.NOT_FOUND)

	if request.method == 'DELETE':
		try:
			cr.execute(f'DELETE FROM {table} WHERE id = {id_int}')
			if not cr.rowcount:
				return Response(status=HTTPStatus.NOT_FOUND)
		except:
			return Response(status=HTTPStatus.CONFLICT)

		db.commit()

		return Response(status=HTTPStatus.OK)
	else:
		if not _verify_req_body(request, json_fields):
			return Response(status=HTTPStatus.BAD_REQUEST)

		keys = list(json_fields.keys())
		keys.remove(ID) if ID in keys else None

		update_body = ", ".join([f'{key} = {_get_field(request, key)}'
			for key in keys])

		try:
			cr.execute(f'UPDATE {table} SET {update_body} WHERE id = {id_int}')
			if not cr.rowcount:
				return Response(status=HTTPStatus.NOT_FOUND)
		except:
			return Response(status=HTTPStatus.CONFLICT)

		db.commit()

		return Response(status=HTTPStatus.OK)
