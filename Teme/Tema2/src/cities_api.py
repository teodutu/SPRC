from flask import Blueprint, request

from server_utils import *


cities_api = Blueprint('cities_api', __name__)


@cities_api.route('/api/cities', methods=['GET', 'POST'])
def handle_get_post():
	return get_post(
		CITIES_DB,
		request,
		[COUNTRY_ID, NAME],
		{COUNTRY_ID: [int], NAME: [str], LAT: [float, int], LONG: [float, int]}
	)


@cities_api.route('/api/cities/<id>', methods=['DELETE', 'PUT'])
def handle_del_put(id):
	return del_put(
		CITIES_DB,
		id,
		request,
		{COUNTRY_ID: [int], ID: [int], NAME: [str],LAT: [float, int],
			LONG: [float, int]}
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

	cr.execute(f'SELECT * FROM {CITIES_DB} WHERE {COUNTRY_ID} = {country_id}')
	return make_get_response([ID, COUNTRY_ID, NAME, LAT, LONG], cr.fetchall())
