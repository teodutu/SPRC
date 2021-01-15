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

package example2;

import java.io.Serializable;

/**
 * O inregistrare referinta de nume pentru identificarea unui peer din cadrul sistemului.
 *
 */
public class LocalAddress implements Serializable {
	static final long serialVersionUID = 7471069187532416589L;
	
	// adresa corespunzatoare
	public final String address;
	
	// portul pe care clientul asculta primirea de date
	public final int port;
	
	/**
	 * Constructorul.
	 * @param address Adresa corespunzatoare intrarii
	 * @param port Portul corespunzator intrarii
	 */
	public LocalAddress(String address, int port) {
		this.address = address;
		this.port = port;
	}
	
	/**
	 * Suprascrierea metodei equals a clasei Object. 
	 * Necesara pentru verificari de egalitate in referiri in diverse structuri de date.
	 */
	public boolean equals(Object o) {
		if (!(o instanceof LocalAddress)) return false;
		LocalAddress a = (LocalAddress)o;
		if (address != null && a.address == null) return false;
		if (address == null && a.address != null) return false;
		if (address == null && a.address == null) return (port == a.port);
		return (address.equals(a.address) && port == a.port);
	}
	
	/**
	 * Suprascrierea metodei toString in scopul obtinerii unui text afisabil corespunzator intrarii.
	 */
	public String toString() {
		return address+":"+port;
	}
} // end of class LocalAddress


