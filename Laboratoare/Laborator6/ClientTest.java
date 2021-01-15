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
		"http://google.com",
		"http://www.yahoo.com",
		"http://acs.pub.ro/~cpop/orare_sem1",
		"http://acs.pub.ro/~cpop/orare_sem2",
		"http://neverssl.com",
		"http://www.baidu.com"
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
		if (poz >= size.length()) {
			return nr;
		}

		return switch (size.substring(poz)) {
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
		if (poz >= time.length()) {
			return nr;
		}

		return switch (time.substring(poz)) {
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

		System.out.println(
			"Client started with maxStorageSpace=" + size
				+ " and maxTimeToKeep=" + time
		);

		WebCache cache = new WebCache(size, time);

		// Cerem "http://acs.pub.ro/~cpop/orare_sem1",
		// "http://acs.pub.ro/~cpop/orare_sem2",
		// si "http://www.baidu.com"
		cache.requestURL(urls[2]);
		cache.requestURL(urls[3]);
		cache.requestURL(urls[2]);
		cache.requestURL(urls[2]);
		cache.requestURL(urls[5]);
		cache.requestURL(urls[2]);
		// Frecventele sunt:
		// http://acs.pub.ro/~cpop/orare_sem1: 4
		// http://acs.pub.ro/~cpop/orare_sem2: 1
		// http://www.baidu.com: 1

		// Cerem "http://neverssl.com"; se elimina "http://www.baidu.com" din
		// cache
		cache.requestURL(urls[4]);
		cache.requestURL(urls[3]);
		cache.requestURL(urls[4]);
		// Frecventele sunt:
		// http://acs.pub.ro/~cpop/orare_sem1: 4
		// http://acs.pub.ro/~cpop/orare_sem2: 2
		// http://neverssl.com: 2

		// Cerem "http://www.yahoo.com"; inca este loc in cache
		// Facem multe cereri pentru a-i creste frecventa
		cache.requestURL(urls[1]);
		cache.requestURL(urls[1]);
		cache.requestURL(urls[1]);
		cache.requestURL(urls[1]);
		cache.requestURL(urls[1]);
		// Frecventele sunt:
		// "http://www.yahoo.com": 5
		// http://acs.pub.ro/~cpop/orare_sem1: 4
		// http://acs.pub.ro/~cpop/orare_sem2: 2
		// http://neverssl.com: 2

		// Cerem "http://google.com"; pagina este mare si se scot pagini din
		// cache, in ordinea frecventelor:
		// - "http://neverssl.com",
		// - "http://acs.pub.ro/~cpop/orare_sem2",
		// - "http://acs.pub.ro/~cpop/orare_sem1"
		cache.requestURL(urls[0]);
	}
} // end of class ClientTest
