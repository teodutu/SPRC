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

package example1;

import java.util.Random;

/**
 * Clasa de test pentru cache-ul web.
 *
 */
public class ClientTest {

	// cateva adrese web predefinite
	private static final String[] urls = {
		"http://google.com", "http://www.yahoo.com", "http://www.pub.ro", "http://neverssl.com/", "http://www.baidu.com/"
	};

	/**
	 * Metoda auxiliara ce converteste dimensiunea exprimata in diverse unitati de masura in valoarea corespunzatoare in B
	 * @param size
	 * @return
	 */
	private static long convertSize(String size) {
		StringBuilder buf = new StringBuilder();
		int poz = 0;

		for (char c : size.toCharArray()) {
			if (!Character.isDigit(c)) {
				break;
			}

			buf.append(c);
			poz++;
		}

		long nr = Long.parseLong(buf.toString());
		if (poz >= size.length()) return nr;
		String unit = size.substring(poz);

		return switch (unit) {
			case "KB" -> nr * 1024L;
			case "MB" -> nr * 1024L * 1024L;
			case "GB" -> nr * 1024L * 1024L * 1024L;
			default -> nr;
		};
	}

	/**
	 * Metoda ce converteste timpul exprimat in diverse unitati de masura in valoarea corespunzatoare exprimata in ms
	 * @param time
	 * @return
	 */
	private static long convertTime(String time) {
		StringBuilder buf = new StringBuilder();
		int poz = 0;

		for (char c : time.toCharArray()) {
			if (!Character.isDigit(c)) {
				break;
			}

			buf.append(c);
			poz++;
		}

		long nr = Long.parseLong(buf.toString());
		if (poz >= time.length()) return nr;
		String unit = time.substring(poz);

		return switch (unit) {
			case "s" -> nr * 1000L;
			case "m" -> nr * 1000L * 1000L;
			case "h" -> nr * 1000L * 1000L * 1000L;
			default -> nr;
		};
	}

	/**
	 * Functia main.
	 * @param args
	 */
	public static void main(String args[]) {
		if (args.length < 2) {
			System.err.println("Usage: client <size> <time>");
			System.exit(-1);
		}

		long size = convertSize(args[0]);
		long time = convertTime(args[1]);

		System.out.println("Client started with maxStorageSpace="+size+" and maxTimeToKeep="+time);

		WebCache cache = new WebCache(size, time);
		Random r = new Random(System.currentTimeMillis());

		while (true) { // incercam cateva citiri aleatoare de adrese folosind cache-ul web
			int poz = r.nextInt(urls.length);
			String page = cache.requestURL(urls[poz]);
			System.out.println(page);

			try {
				Thread.sleep(1000);
			} catch (Throwable t) { }
		}
	}

} // end of class ClientTest
