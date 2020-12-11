from flask import Flask

from cities_api import cities_api
from countries_api import countries_api
from temperatures_api import temperatures_api
from server_utils import *


server = Flask(__name__)

server.register_blueprint(cities_api)
server.register_blueprint(countries_api)
server.register_blueprint(temperatures_api)


if __name__ == "__main__":
	server.run('0.0.0.0', debug=False)
