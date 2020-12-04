from datetime import datetime
from flask import Blueprint, json, Response, request
from http import HTTPStatus

from server_utils import *


temperatures_api = Blueprint('temperatures_api', __name__)


def _get_dates(request):
	from_date = datetime.strptime(
		request.args.get(FROM, default='1000-01-01', type=str),
		'%Y-%m-%d'
	)
	until_date = datetime.strptime(
		request.args.get(UNTIL, default='9999-12-31', type=str),
		'%Y-%m-%d'
	).replace(hour=23, minute=59, second=59)

	return from_date, until_date


def _get_local_temps(table, id, keys, query):
	try:
		_ = int(id)
	except ValueError:
		return Response(
			status=HTTPStatus.OK,
			response=json.dumps([]),
			mimetype=RESPONSE_CONTENT_TYPE
		)

	cr.execute(query)
	return make_get_response(keys, cr.fetchall())


@temperatures_api.route('/api/temperatures', methods=['POST'])
def handle_post():
	return get_post(
		TEMPERATURES_DB,
		request,
		[TIMESTAMP, CITY_ID],
		{VALUE: [float, int], CITY_ID: [int]}
	)


@temperatures_api.route('/api/temperatures/<id>', methods=['DELETE', 'PUT'])
def handle_del_put(id):
	return del_put(
		TEMPERATURES_DB,
		id,
		request,
		{ID: [int], VALUE: [float, int], CITY_ID: [int]}
	)


@temperatures_api.route('/api/temperatures', methods=['GET'])
def handle_get():
	lat = request.args.get(LAT, type=float)
	long = request.args.get(LONG, type=float)
	from_date, until_date = _get_dates(request)

	conds = []
	if lat:
		conds.append(f'{LAT} = {lat}')
	if long:
		conds.append(f'{LONG} = {long}')

	if conds:
		subclause = (f' AND {CITY_ID} in (SELECT {ID} FROM {CITIES_DB} WHERE '
			+ ' AND '.join(conds) + ')')
	else:
		subclause = ''

	cr.execute(f"""SELECT {ID}, {VALUE}, DATE_FORMAT({TIMESTAMP}, '%Y-%m-%d')
		FROM {TEMPERATURES_DB} WHERE
			{TIMESTAMP} BETWEEN '{from_date}' AND '{until_date}'
			{subclause}
	""")
	return make_get_response([ID, VALUE, TIMESTAMP], cr.fetchall())


@temperatures_api.route('/api/temperatures/cities/<id>', methods=['GET'])
def handle_get_cities(id):
	from_date, until_date = _get_dates(request)

	return _get_local_temps(CITIES_DB, id, [ID, VALUE, TIMESTAMP],
		f"""SELECT {ID}, {VALUE}, DATE_FORMAT({TIMESTAMP}, '%Y-%m-%d')
			FROM {TEMPERATURES_DB} WHERE
				{CITY_ID} = {id}
				AND {TIMESTAMP} BETWEEN '{from_date}' AND '{until_date}'
		"""
	)


@temperatures_api.route('/api/temperatures/countries/<id>', methods=['GET'])
def handle_get_countries(id):
	from_date, until_date = _get_dates(request)

	return _get_local_temps(COUNTRIES_DB, id, [ID, VALUE, TIMESTAMP],
		f"""SELECT {ID}, {VALUE}, DATE_FORMAT({TIMESTAMP}, '%Y-%m-%d')
			FROM {TEMPERATURES_DB} WHERE
				{TIMESTAMP} BETWEEN '{from_date}' and '{until_date}'
				and {CITY_ID} IN
					(SELECT {ID} FROM {CITIES_DB} WHERE {COUNTRY_ID} = {id})
		"""
	)
