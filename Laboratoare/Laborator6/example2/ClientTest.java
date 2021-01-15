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

import java.util.Random;

/**
 * Un client de test pentru memoria partajata.
 *
 */
public class ClientTest {

	// Nume de variabile ce sunt scrise sau citite
	public static final String varNames[] = new String[] { "a", "b", "c", "d", "e" };

	public static void main(String args[]) {

		String adr = null;
		int port = -1;
		if (args.length >= 2) {
			adr = args[0];
			try {
				port = Integer.parseInt(args[1]);
			} catch (Exception e) {
				e.printStackTrace();
				System.exit(-1);
			}
		}
		Random r = new Random(System.currentTimeMillis());
		try {
			SharedMemoryProcess process = new SharedMemoryProcess(adr, port);
			SharedMemoryReplica replica = process.getReplica();
			Integer i = r.nextInt(1000);
			replica.write("a", i);
			System.out.println(System.currentTimeMillis()+" Write a:= "+i);
			Thread.sleep(1000);
			i = (Integer)replica.read("a");
			System.out.println(System.currentTimeMillis()+" Read a:= "+i);
			Thread.sleep(1000);

			while (true) {
				int poz = r.nextInt(varNames.length);
				int op = r.nextInt(100);
				if (op < 50) { // readOp
					i = (Integer)replica.read(varNames[poz]);
					System.out.println(System.currentTimeMillis()+" Read "+varNames[poz]+":= "+i);
				} else {
					i = r.nextInt(1000);
					replica.write(varNames[poz], i);
					System.out.println(System.currentTimeMillis()+" Write "+varNames[poz]+":= "+i);
				}
				Thread.sleep(1000);
			}
		} catch (Throwable t) {
			t.printStackTrace();
			System.exit(-1);
		}
	}

} // end of class ClientTest
