/**
	Sisteme de programe pentru retele de calculatoare
	
	Copyright (C) 2008 Ciprian Dobre & Florin Pop
	Univerity Politehnica of Bucharest, Romania

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 */

Laborator 3. Consistenta datelor si toleranta la defecte.

     Listing 2. Exemplu de implementare a unei memorii partajate in Java.
     
     In cadrul exemplului se ilustreaza o posibila modalitate de implementare a unei memorii partajate. Avantajul abordarii este ca
     nu exista nici un single-point-of-failure, toate procesele cunoscand aceeasi copie a memoriei partajate si aceasi tabela de topologie
     curenta a proceselor existente. O variabila are un owner ce este raspunzator de toate actualizarile respectivei variabile. In momentul
     in care un proces pica se incepe un procedeu de votare prin care se alege un alt proces care preia drepturile asupra tuturor variabilelor
     detinute anterior de procesul picat. Astfel, toate procesele cunoscand aceeasi copie a memoriei partajate, chiar daca un singur proces
     ramane pana la final in viata datele memoriei partajate nu se pierd. De asemenea orice actualizare a unei variabile este automat vazuta
     de toate celelalte procese, deci si consistenta datelor este convervata.
     
     Pentru mai multe detalii se pot consulta explicatiile din fisierele sursa. Rularea aplicatiei se realizeaza cu ant.
     
 