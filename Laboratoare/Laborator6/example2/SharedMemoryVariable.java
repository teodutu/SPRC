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
 * O variabila partajata.
 *
 */
public class SharedMemoryVariable implements Serializable {
	static final long serialVersionUID = 7471069187532416589L;
	
	// detinatorul curent al variabilei partajate
	public LocalAddress owner;
	// numele variabilei
	public final String name;
	// valoarea curenta a variabilei
	public Serializable value;
	
	/**
	 * Constructorul.
	 */
	public SharedMemoryVariable(LocalAddress owner, String name) {
		this.owner = owner;
		this.name = name;
	}
	
} // end of class SharedMemoryVariable

