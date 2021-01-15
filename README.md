# SPRC
Sisteme de Programe pentru Retele de Calculatoare - UPB 2020-2021

## Laboratoare
### Laborator 1 - HTTP
#### Task 1
Se trimite un `POST` request catre `https://sprc.dfilip.xyz/lab1/task1/check`
cu headerul `secret2: SPRCisBest`, parametrii `nume=<nume>&grupa=<grupa>` si
datele `secret: SPRCisNice` encodate ca `form`. Se obtine urmatorul raspuns,
ce contine si tokenul JWT cerut:
```json
{
    "status": "ok",
    "proof": "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJ3aG8iOnsibnVtZSI6WyJUZW9kb3ItU3RlZmFuIER1dHUiXSwiZ3J1cGEiOlsiMzQxQzMiXX0sImRpZCI6InRhc2sxIn0.zP6gL5EOQBp-jc79FBa1Sughh2R-wwrA86pnMvQUGgk"
}
```

#### Task 2
Se trimite o cerere `POST` catre `https://sprc.dfilip.xyz/lab1/task2` care
contine JSON-ul urmator:
```json
{
	"username": "sprc",
	"password": "admin",
	"nume": <nume>
}
```
Se primeste urmatorul raspuns:
```json
{
    "status": "ok",
    "proof": "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJ3aG8iOnsibnVtZSI6IlRlb2Rvci1TdGVmYW4gRHV0dSJ9LCJkaWQiOiJ0YXNrMiJ9.k_kcAPhC78_NmVEq1u2VTphmSPZFusWY8igZnp47qLc"
}
```

#### Task 3
Se foloseste requestul de la [Task 2](#task-2) pentru a se initia o sesiune
catre `https://sprc.dfilip.xyz/lab1/task3/login`. A
doua cerere HTTPS din aceasta sesiune reprezinta o cerere `GET` catre
`https://sprc.dfilip.xyz/lab1/task3/check`, in urma careia se primeste
raspunsul:
```json
{
    "status": "ok",
    "proof": "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJ3aG8iOnsibnVtZSI6IlRlb2Rvci1TdGVmYW4gRHV0dSJ9LCJkaWQiOiJ0YXNrMyJ9.K8Oxp2wINIkScaOiepDnSbMmLz_bPS5lsugFe1mBHzM"
}
```


### Laborator 2 - RPC
Se implementeaza perechi de clienti si servere *RPC* folosind API-ul `rpc` din
POSIX.

#### Get Sum
Serverul calculeaza suma a 2 numere. Duh...

#### Check
Se implementeaza doar clientul care trimite catre `sprc2.dfilip.xyz` un *RPC* ce
are ca parametru o structura care contine 2 stringuri: `nume` si `grupa`, iar
serverul intoarce `Well done <nume> (<grupa>)`.

#### Closest Prime
Clientul ii da serverului un numar, iar serverul intoarce numarul prim cel mai
apropiat de cel primit de la client. Serverul foloseste
[ciurul lui Atkin](https://en.wikipedia.org/wiki/Sieve_of_Atkin) (luat din
[tema la AA](https://github.com/teodutu/PA/blob/master/Teme/Tema1_AA/tester.cpp)
) pentru a verifica daca un numar este prim in `O(1)`.

#### Remote Shell
Serverul actioneaza ca un shell remote permitand clientilor sa ruleze comenzi pe
acesta. Fiind vorba despre *RPC*, shellul este *stateless*, iar lungimea
outputului comenzii este setata de catre client (maximum `8192` octeti).


### Laborator 3 - REST Web Server
Se implementeaza, folosind [Flask](https://flask.palletsprojects.com/en/1.1.x/),
un server web care gestioneaza o colectie de filme si suporta metodele `GET`,
`POST`, `PUT`, `DELETE`.


### Laborator 4 - Docker
Nimicuri cu dockere. Pana acum era mai misto.

#### Task 1
Se descarca fisierele necesare de
[aici](https://static.dfilip.xyz/sprc_lab_docker_task1.zip), dupa care se
creeaza dockerul cu `sudo docker build . -t task1`. Apoi se ruleaza dockerul
creat legandu-l la portul `8080` al hostului astfel:
```bash
sudo docker run -it -p 8080:80 task1
```

#### Task 2
Se creeaza o suita de servicii capabile sa stocheze date persistent, folosind
`docker-compose`.


### Laborator 5 - MQTT
Se foloseste protocolul [MQTT](https://mqtt.org/) pentru a simula un chat de
*IRC* practic :))) Protocolul functioneaza pe baza modelului
*Publisher-Subscriber*. Topicul la care clientul se aboneaza este
`chat/sprc/#`, iar cel pe care publica este `chat/sprc/<NumeStudent>`.

Alt lab degeaba.


### Laborator 6 - Web Cache in Java
Primul lab mai de Doamne-ajuta. Se modifica cache-ul **LRU** dat in schelet cu
unul **LFU** si se adauga o serie de cereri web care sa demonstreze aceasta
politica.



## Teme
### Tema 1 - RPC
Se implementeaza o aplicatie client-server care gestioneaza o baza de date.
Serverul retine baza de date atat in fisiere cat si in memorie, iar clientii
pot efectua actiuni asupra acesteia prin intermediul unor *RPC*-uri descrise
in `README`-ul temei.


### Tema 2 - Docker + microservicii
Se implementeaza o aplicatie web (kms) care gestioneaza o baza de date ce
contine temperaturi masurate la anumite coordonate. Fiecare coordonata e
asociata unui oras, care si el se afla la randul lui intr-o tara. Aplicatia
suporta metodele `GET`, `POST`, `PUT` si `DELETE` ce permit modificarea
tabelelor ce contin tarile, orasele si temperaturile, cu mentiunea ca stergerile
se fac in cascada.

De asemenea, aplicatia contine si un GUI
([mysql-workbench](https://hub.docker.com/r/linuxserver/mysql-workbench)) cu
care se poate inspecta baza de date. Toate componentele sunt create in cadrul
unor containere folosind `docker-compose`.

Pe scurt, tema e un jeg in care e mai mult *BD* si bulaneala in
`docker-compose.yml` decat cod scris pe bune...

### Tema 3 - Tot docker...
Cam la fel ca tema 2, dar cu *InfluxDB* in loc de *MySQL* si cu
[Grafana](https://grafana.com/) in loc de *mysql-workbench*. Practic aceeasi
mizerie ca la tema 2, dar cu un cancer extra, adica *Grafana*... De ce pula mea
facem la *SPRC* mai mult BD decat orice altceva?
