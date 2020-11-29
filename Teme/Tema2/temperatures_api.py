from datetime import datetime
from flask import Blueprint, json, Response, request
from http import HTTPStatus
from math import inf

from server_utils import *


temperatures_api = Blueprint('temperatures_api', __name__)


@temperatures_api.route('/api/temperatures', methods=['POST'])
def handle_post():
	return get_post(
		TEMPERATURES_DB,
		request,
		[TIMESTAMP, CITY_ID],
		[CITY_ID, VALUE]
	)


@temperatures_api.route('/api/temperatures/<id>', methods=['DELETE', 'PUT'])
def handle_del_put(id):
	return del_put(
		TEMPERATURES_DB,
		id,
		request,
		[TIMESTAMP, CITY_ID],
		[ID, CITY_ID, VALUE]
	)


@temperatures_api.route('/api/temperatures', methods=['GET'])
def handle_get():
	lat = request.args.get(LAT, default=0.0, type=float)
	long = request.args.get(LONG, default=inf, type=float)
	fromDate = datetime.strptime(
		request.args.get(FROM, default='0001-01-01', type=str),
		'%Y-%m-%d'
	)
	untilDate = datetime.strptime(
		request.args.get(UNTIL, default='9999-12-31', type=str),
		'%Y-%m-%d'
	)

	# TODO: ia din BD cu param de mai sus

	dummy_resp = [{ID: 1, VALUE: 11.1, TIMESTAMP: datetime.now().strftime('%Y-%m-%d')}]
	return Response(
		status=HTTPStatus.OK,
		response=json.dumps(dummy_resp),
		mimetype=RESPONSE_CONTENT_TYPE
	)


@temperatures_api.route('/api/temperatures/cities/<id>', methods=['GET'])
def handle_get_cities(id):
	return get_temps_area(CITIES_DB, id, request)


@temperatures_api.route('/api/temperatures/countries/<id>', methods=['GET'])
def handle_get_countries(id):
	return get_temps_area(COUNTRIES_DB, id, request)
