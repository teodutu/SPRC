from flask import Blueprint, request

from server_utils import *


cities_api = Blueprint('cities_api', __name__)


@cities_api.route('/api/cities', methods=['GET', 'POST'])
def handle_get_post():
	return get_post(
		CITIES_DB,
		request,
		[COUNTRY_ID, NAME],
		[ID, COUNTRY_ID, NAME, LAT, LONG],
	)


@cities_api.route('/api/cities/<id>', methods=['DELETE', 'PUT'])
def handle_del_put(id):
	return del_put(
		CITIES_DB,
		id,
		request,
		[COUNTRY_ID, NAME],
		[ID, COUNTRY_ID, NAME, LAT, LONG]
	)


@cities_api.route('/api/cities/country/<id>', methods=['GET'])
def handle_get_country_id(id):
	try:
		country_id = int(id)
	except ValueError:
		return Response(
			status=HTTPStatus.OK,
			response=json.dumps([]),
			mimetype=RESPONSE_CONTENT_TYPE
		)

	# TODO cauta in BD

	dummy_json = [{ID: -1, NAME: "plm", LAT: 1.1, LONG: 2.2}]
	return Response(
		status=HTTPStatus.OK,
		response=json.dumps(dummy_json),
		mimetype=RESPONSE_CONTENT_TYPE
	)
