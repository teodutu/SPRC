# Laboratot 6 - Web Cache LFU

Pentru a implementa politica *LFU*, am adugat clasei `WebCacheEntry` un nou
camp, `frequency`, care se incrementeaza la fiecare accesare a paginii. Pe baza
acestuia, atunci cand este nevoie sa se elimine o pagina din cache, se va
alege cea care are valoarea minima in acest camp.


## Testare
In metoda `main()` a fisierului `ClientTest.java` au fost modifiacate cererile
de pagini web astfel incat sa se ilustreze politica *LFU*. Comentariile din
jurul fiecarui grup de cereri mentioneaza paginile eliminate (atunci cand se
intampla acest lucru) si motivul pentru care acestea sunt eliminate, precum
si frecventele fiecarei pagini din cache.

Ruland aplicatia folsind `ant execute`, se observa ca aceasta se comporta asa
cum este descris in comentariile antementionate.


## Precizare
In `WebCache.java` din exemplul 1, linia 99 trebuie modificata din
```java
cacheFile.removeEntry(entry);
```
in
```java
cacheFile.removeEntry(wce);
```
Daca scheletul e vechi din 2008, chiar nu a observat nimeni asta in 13 ani? :(
