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

import java.io.IOException;
import java.io.Serializable;
import java.util.Hashtable;

/**
 * O copie serializabila a intrarilor curente existente in memoria partajata.
 *
 */
public class SharedMemoryReplica implements Serializable {

	static final long serialVersionUID = 7471069187532416589L;
	
	// variabilele curente (nume variabila vs. SharedMemoryVariable)
	public Hashtable variables = new Hashtable();
	
	// procesul ce detine replica 
	public transient SharedMemoryProcess process;
	
	/**
	 * Constructorul.
	 */
	public SharedMemoryReplica() {
	}
	
	/**
	 * Metoda prin care este citita valoarea curenta a unei variabile.
	 * @param varName Numele variabilei.
	 * @return Valoarea curenta a variabilei.
	 */
	public Object read(String varName) {
		if (variables == null || !variables.containsKey(varName)) { // daca nu exista variabila
			return null;
		}
		SharedMemoryVariable sh = (SharedMemoryVariable)variables.get(varName);
		return process.read(sh); // deleaga citirea propriu-zisa procesului corespunzator
	}
	
	/**
	 * Metoda prin care este scrisa o noua valoare corepunzand unei anumite variabile
	 * @param varName Numele variabilei.
	 * @param varValue Valoarea acesteia.
	 */
	public void write(String varName, Serializable varValue) {
		SharedMemoryVariable var = null;
		if (variables.containsKey(varName)) {
			var = (SharedMemoryVariable)variables.get(varName);
		} else {
			var = new SharedMemoryVariable(process.localAddress, varName);
			variables.put(varName, var);
		}
		var.value = varValue; // setam noua valoare locala
		process.write(var); // delegam scrierea procesului 
	}
		
	private void writeObject(java.io.ObjectOutputStream stream) throws IOException, ClassNotFoundException {
		synchronized (variables) {
			stream.writeObject(variables);
		}
	}

	private void readObject(java.io.ObjectInputStream stream) throws IOException, ClassNotFoundException {
		variables = (Hashtable)stream.readObject();
	}
} // end of class SharedMemoryReplica


