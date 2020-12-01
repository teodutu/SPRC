# Tema 2 - Servicii Web


## Rulare
Tot ce e necesar pentru rulare este comanda rulata din radacina proiectului:
```
docker-compose up --build
```

## Structura
### API
Serverul web ce expune *API*-ul este implementat in *Python* folosind biblioteca
`Flask`.

Fisierele sursa ale serverul se afla, impreuna cu fisierele `Dockerfile` si
`requirements.txt` in directorul `src/`. Pentru ca serverul acesta sa se poata
conecta la *MySQL*, este nevoie sa astepte ca baza de date sa porneasca, motiv
pentru care in `Dockerfile` se ruleaza scriptul `start_server.sh`, care face
serverul sa astepte (testand folosind `netcat`) pana cand baza de date este
initializata.

Fiecare parte din *API* (de ex. `/api/countries`) este implementata ca un
`Blueprint` intr-un fisier cu denumire specifica (de ex. *API*-ul anterior este
implementat in fisierul `countries_api.py`).

### Baza de date
Aceasta este de tip *MySQL* si foloseste pentru initializare scriptul
`./db/init-db.sql`, iar directorul `db/` este locul in care va fi salvat volumul
ce va stoca persistent baza de date.

Pentru conectare, se vor folosi credentialele:
```
username: meteo-admin
password: meteo
```

### Utilitarul de gestiune
Utilitarul folosit este
[mysql-workbench](https://hub.docker.com/r/linuxserver/mysql-workbench). Pentru
conectarea la baza de date (folosind credentialele din sectiunea anterioara)
este necesar ca parola sa fie oferita din *Keychain*, pentru securitate.

## Stergere
Scriptul `cleanup.sh` disponibil in radacina proiectului sterge din sistem toate
resursele descarcate sau create de aplicatie.

## Testare
Suita de teste cu care a fost verificata aplicatia se gaseste in fisierul
`tests/Tema2_SPRC_tests.postman_collection.json`.
