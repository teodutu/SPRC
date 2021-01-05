# Tema 3 - Microservicii


## Rulare
Pentru a crea imaginea serverului (singura componenta a aplicatiei care nu este
o imagine de pe [hub.docker.com](hub.docker.com)) si rula aplicatia se poate
folosi scriptul `run.sh`. Dupa ce imaginea serverului a fost creata, aplicatia
poate fi pornita cu comanda `docker stack deploy -c stack.yml <nume_aplicatie>`
Aplicatia poate fi oprita folosind comanda `docker stack rm <nume_aplicatie>`.

Inainte de rulare, trebuie definita variabila de mediu `SPRC_DVP`, ce va
reprezenta directorul in care se va salva volumul bazei de date. Mai multe
detalii sunt prezentate in [sectiunea aferenta bazei de date](#baza-de-date)


## Structura
Aplicatia contine cele 4 componente specificate in enunt:
- Brokerul [eclipse-mosquitto](https://hub.docker.com/_/eclipse-mosquitto)
- Arbitrul implementat in fisierul `src/server.py`
- Baza de date [InfluxDB](https://hub.docker.com/_/influxdb)
- Utilitarul de vizualizare a bazei de date,
[Grafana](https://hub.docker.com/r/grafana/grafana/)

### Brokerul
Am folosit imaginea *eclipse-mosquitto* intrucat eram deja relativ familiar cu
ea de la laboratorul de *MQTT*. Brokerul expune portul `1883`.

### Arbitrul
Este in acelasi timp un client al brokerului si un server pentru baza de date.
Se aboneaza la toate topicurile brokerului (`#`), preia mesajele trimise de
clienti si le introduce in baza de date folosind schema de mai jos. Am folosit
aceasta schema deoarece aveam nevoie sa accesez valorile trimise de clientii
brokerului intr-un format grupat dupa `statie` si `metrica`, drept care am
folosit aceasta combinatie pentru `measurement` si am trimis mesajele de la
clienti sub forma unei liste in care fiecare pereche *cheie - valoare* respecta
schema obiectului de mai jos.
```json
{
	"measurement": "<station>.<key>",
	"tags": {
		"location": <location>,
		"station": <station>
	},
	"time": <timestamp>,
	"fields": {
		"value": <value>
	}
}
```
Am ales aceste taguri deoarece e nevoie de locatia `"UPB"` pentru dashboardul
"UPB IoT Data" si de statie pentru dashboardul "Battery Dashboard".

### Baza de date
Se foloseste un script (`db/init-db.sh`) care creeaza baza de date `iot_data` cu
timpul de retentie a datelor infinit. Volumul folosit de aceasta se afla in
locatia `${SPRC_DVP}/influxdb-storage`, unde variabila de mediu `SPRC_DVP`
trebuie definita a priori.

### Grafana
Dashboardurile sunt configurate in folderul `grafana-provisioning/dashboards`.
Credentialele de logare sunt specificate in fisierul `envs/grafana.env`.
Eventualele modificari facute de utilizator sunt salvate in volumul
`${SPRC_DVP}/grafana-storage`.


## Testare
Pentru a se putea testa aplicatia prin intermediul dashboardurilor oferite de
*Grafana*, se poate folosi clientul definit in `clients/test_client.py`. Acesta
adauga metricile `BAT`, `HUM` si `TEMP` de la locatia `"UPB"` cu valori
aleatoare din cate un interval pentru fiecare metrica. Statiile sunt tot valori
aleatoare din multimea `{"A", "B", "C"}`.
