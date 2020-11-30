from flask import Flask

from cities_api import cities_api
from countries_api import countries_api
from temperatures_api import temperatures_api
from server_utils import *


server = Flask(__name__)

server.register_blueprint(cities_api)
server.register_blueprint(countries_api)
server.register_blueprint(temperatures_api)


def main():
	cr.execute(f'DROP DATABASE {DB_NAME}')

	cr.execute(f"SHOW DATABASES LIKE '{DB_NAME}'")
	if cr.fetchall() == ():
		cr.execute(f'CREATE DATABASE {DB_NAME}')

	cr.execute(f'USE {DB_NAME}')
	cr.execute('SHOW TABLES')

	if cr.fetchall() == ():
		# TODO: Foreign keys
		cr.execute(f"""CREATE TABLE {COUNTRIES_DB} (
            {ID} INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
            {NAME} VARCHAR(128) UNIQUE,
            {LAT} DOUBLE(8, 3) NOT NULL,
            {LONG} DOUBLE(8, 3) NOT NULL
        )""")
		cr.execute(f"""CREATE TABLE {CITIES_DB} (
            {ID} INT(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
            {COUNTRY_ID} INT(11),
            {NAME} VARCHAR(128),
            {LAT} DOUBLE(8, 3) NOT NULL,
            {LONG} DOUBLE(8, 3) NOT NULL,
            CONSTRAINT UC_City UNIQUE ({COUNTRY_ID}, {NAME})
		)""")
		cr.execute(f"""CREATE TABLE {TEMPERATURES_DB} (
            {ID} INT(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
            {VALUE} DOUBLE(8, 3) NOT NULL,
            {TIMESTAMP} VARCHAR(11),
            {CITY_ID} VARCHAR(128),
            CONSTRAINT UC_Temp UNIQUE({TIMESTAMP}, {CITY_ID})
        )""")

	server.run('0.0.0.0', debug=True)  # TODO: modifica IP?

if __name__ == "__main__":
	main()
