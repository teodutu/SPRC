# Tema 1 - RPC + DB
Se implementeaza o baza de date care este retinuta de un server si la care
clientii au acces prin *Remote Procedure Calls*.

Pentru a se conecta la server si a-si accesa baza de date, clientii folosesc
comanda `login <nume>`, careia serverul le raspunde cu o cheie de sesiune.
Aceasta cheie este trimisa serverului de catre client la fiecare *RPC*, pentru
ca serverul sa poata identifica in mod corect clientul care a efectual acel
*RPC*.


## Serverul
Retine bazele de date a clientului atat in RAM cat si pe disc. Pentru stocarea
volatila, in RAM, utilizeaza un `unordered_map` in care foloseste cheile de
sesiune trimise de clienti pentru a identifica bazele de date ale acestora.
O baza de date a unui client este ea insasi tot un `unordered_map` indexat
dupa id-ul fiecarei intrari.

### Statistici
Statisticile atat la nivelul unei intregi baze de date, cat si la nivelul unei
intrari sunt calculate (sau recalculate) la efectuarea *RPC*-urilor 1 `add` si
`update`, si nu atunci cand aceste statistici sunt cerute de client prin
*RPC*-uri `get_stat*`. Astfel, *RPC*-urile `get_stat*` intorc raspunsuri in
`O(1)`.

Acest lucru se intampla deoarece cel mai adesea bazele de date sunt accesate
mult mai mult pentru citiri decat pentru scrieri. De asemenea, serverul intoarce
un raspuns de tip `ERROR` atunci cand un client cere statistici pentru o baza de
data goala, pantru o intrare inexistenta, sau pentru una fara elemente.

### Load si Store
Aceste comenzi date de clienti citesc baza de date de pe disc, aducand-o in RAM,
respectiv scriu baza de date din RAM pe disc. Serverul refuza sa scrie pe disc
o baza de date goala sau intrari fara valori, intorcand un raspuns `ERROR` in
ambele cazuri.

De asemenea, daca in momentul in care un client trimite un *RPC* `load`,
serverul are deja in RAM o baza de date cu cel putin o intrare (creata prin
*RPC*-uri `add` sau `update` sau printr-un `load` anterior), atunci `load` va
intoarce `ERROR`.


## Clientul
Clientii implementati sunt unii de test, care citesc comenzi de la `stdin`,
apeleaza *RPC* urile aferente acestora si afiseaza rezultatele primite de la
server la `stdout` sau la `stderr` (in cazul raspunsurilor cu statusul `ERROR`).

Pe langa comenzile standard, un client poate interpreta si comenzile
`sleep <seconds>`, care face clientul inactiv timp de `seconds` secunde, si
`exit`, care inchide clientul.


## Compilare si testare
Compilarea serverului si a clientului se poate face prin intermediul fisierului
`Makefile` disponibil in directorul `src/`. Testarea acestora se poate face
fie manual (dand comenzi clientilor din linia de comanda), fie prin intermediul
scriptului `run_tests.sh`, care ruleaza testele din directorul `tests/` si
salveaza ceea ce afiseaza clientul si serverul in fisiere aflate in folderul
`out`, pentru ca mai apoi sa compare aceste rezultate cu datele de referinta
aflate in folderul `refs`.
