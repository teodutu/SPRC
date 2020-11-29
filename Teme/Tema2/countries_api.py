from flask import Blueprint, json, Response, request
from http import HTTPStatus

from server_utils import *


countries_api = Blueprint('countries_api', __name__)


@countries_api.route('/api/countries', methods=['GET', 'POST'])
def handle_get_post():
	return get_post(COUNTRIES_DB, request, [NAME], [NAME, LAT, LONG])


@countries_api.route('/api/countries/<id>', methods=['DELETE', 'PUT'])
def handle_del_put(id):
	return del_put(COUNTRIES_DB, id, request, [NAME], [ID, NAME, LAT, LONG])
