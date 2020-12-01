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
            CONSTRAINT UC_City UNIQUE ({COUNTRY_ID}, {NAME}),
            CONSTRAINT FK_Country FOREIGN KEY ({COUNTRY_ID}) REFERENCES {COUNTRIES_DB} ({ID}) ON DELETE CASCADE
        )""")
        cr.execute(f"""CREATE TABLE {TEMPERATURES_DB} (
            {ID} INT(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
            {VALUE} DOUBLE(8, 3) NOT NULL,
            {TIMESTAMP} DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
            {CITY_ID} INT(11),
            CONSTRAINT UC_Temp UNIQUE ({TIMESTAMP}, {CITY_ID}),
            CONSTRAINT FK_City FOREIGN KEY ({CITY_ID}) REFERENCES {CITIES_DB} ({ID}) ON DELETE CASCADE
        )""")

    server.run('0.0.0.0', debug=True)  # TODO: modifica IP?

if __name__ == "__main__":
    main()
