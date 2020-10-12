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
