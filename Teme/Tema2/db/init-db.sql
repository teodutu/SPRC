CREATE DATABASE IF NOT EXISTS Meteorologie;
USE Meteorologie;


CREATE TABLE IF NOT EXISTS Tari (
    id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    nume VARCHAR(128) UNIQUE,
    lat DOUBLE(8, 3) NOT NULL,
    lon DOUBLE(8, 3) NOT NULL
);

CREATE TABLE IF NOT EXISTS Orase (
    id INT(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
    idTara INT(11),
    nume VARCHAR(128),
    lat DOUBLE(8, 3) NOT NULL,
    lon DOUBLE(8, 3) NOT NULL,
    CONSTRAINT UC_City UNIQUE (idTara, nume),
    CONSTRAINT FK_Country FOREIGN KEY (idTara)
        REFERENCES Tari (id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Temperaturi (
    id INT(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
    valoare DOUBLE(8, 3) NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    idOras INT(11),
    CONSTRAINT UC_Temp UNIQUE (timestamp, idOras),
    CONSTRAINT FK_City FOREIGN KEY (idOras)
        REFERENCES Orase (id) ON DELETE CASCADE
);