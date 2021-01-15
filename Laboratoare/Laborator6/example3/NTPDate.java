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

package example3;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.TimeZone;
import java.util.logging.Level;
import java.util.logging.Logger;

/** 
 * Constructs a Date object sync'ed with the time servers and having GMT time
 */
public class NTPDate extends Date{ 

	private static final long	serialVersionUID	= 8363930011000237062L;

	private static final transient Logger logger = Logger.getLogger("lia.util.ntp.NTPDate");

	private static long lNTPOffset;
	private static long lLastSynchronizationTime = 0;
	private static boolean bFirstRunComplete = false;

	private static final long SECOND    =   1000;
	private static final long MINUTE    =   60*SECOND;
	private static final long HOUR      =   60*MINUTE;

	public static final boolean isSynchronized(){
		// NTP is assumed to be synchronized if the last successfull check was at most 1/2 an hour ago
		return (System.currentTimeMillis() - lLastSynchronizationTime) < 30*MINUTE;
	}

	private static class NTPThread extends Thread{
		private long lastTimeLogged;
		private boolean stopped = false;
		private Thread ownThread = null;
		private static final Object lock = new Object();

		public NTPThread(){
			super(" (ML) NTPThread");
			lastTimeLogged = 0;
			stopped = false;
		}

		public void stopIt() {

			stopped = true;
			synchronized (lock) {
				while (ownThread == null) {
					try {
						lock.wait();
					} catch(Exception ex) { }
				}
			}
			try {
				this.interrupt();
			} catch (Exception ex) { }
		}

		public void run(){

			synchronized (lock) {
				ownThread = this;
				lock.notifyAll();
			}

			while (!stopped){
				long lsleep = 10*SECOND;
				try {
					final NTPClient cl = new NTPClient();
					final long temp = cl.NTPConnect();

					if (cl.NTPok()){
						final long lOld = lNTPOffset;

						final long diff = Math.abs(lNTPOffset - temp);

						if (bFirstRunComplete && (diff > 10*SECOND)){
							// if the time difference is too big then go in that direction in small steps

							int factor = 10;

							if (diff > 24*HOUR){
								logger.log(Level.WARNING, "Something is wrong with the time on this machine. Current time : "+new Date()+" ("+System.currentTimeMillis()+"), old NTP offset : "+lNTPOffset+", new NTP offset : "+temp+", valid responses : "+cl.getValidServersCount());
								factor = 10000;
							}
							else
								if (diff > 1*HOUR)
									factor = 1000;
								else
									if (diff > 5*MINUTE)
										factor = 100;

							lNTPOffset = (long) ( (lNTPOffset*(double)(factor-1) + temp) / factor );
						}
						else{
							lNTPOffset = temp;
						}

						lLastSynchronizationTime = System.currentTimeMillis();

						long lDiff = Math.abs(lNTPOffset - lOld);

						if (lDiff<15)        lsleep = 15*MINUTE;
						else if (lDiff<50)   lsleep = 10*MINUTE;
						else if (lDiff<100)  lsleep =  5*MINUTE;
						else if (lDiff<250)  lsleep =  2*MINUTE;
						else if (lDiff<1000) lsleep =  1*MINUTE;
						else lsleep = 10*SECOND;

						if(bFirstRunComplete) {
							final long now = NTPDate.currentTimeMillis();
							if(lastTimeLogged + 12*HOUR < now) {
								logger.log(Level.INFO, "recalculated offset is : "+lNTPOffset+", "+cl.getValidServersCount()+" NTP replies, now="+new Date(now)+", next NTP sync in "+(lsleep/SECOND)+" sec");
								lastTimeLogged = now;
							} else if(logger.isLoggable(Level.FINE)) {
								logger.log(Level.FINE, "recalculated offset is : "+lNTPOffset+", "+cl.getValidServersCount()+" NTP replies, now="+new Date(now)+", next NTP sync in "+(lsleep/SECOND)+" sec");
							}
						}
					}
					else{
						logger.log(Level.WARNING, "Cannot determine the offset, falling back to the previous value ("+lNTPOffset+").");
					}

				}catch(Throwable t){
					logger.log(Level.WARNING, "NTPThread got exc", t);
				}

				bFirstRunComplete = true;
				try{
					sleep(lsleep);	// aprox. 5 min
				}
				catch (Exception e){
				}
			}
		}

	}

	static Thread ntpt = null;

	static{
		boolean bStartNTP = true;

		try{
			bStartNTP = Boolean.valueOf(System.getProperty("ntp.enabled","true")).booleanValue();
		}
		catch (Throwable t){
		}

		lNTPOffset = 0;
		if (bStartNTP){
			logger.log(Level.CONFIG, "Using NTP to calculate time. To disable it you can set lia.util.ntp.enabled=false (not recommended).");
			bFirstRunComplete = false;
			startThread();
		}
		else{
			logger.log(Level.CONFIG, "NTP is disabled. We recommend setting lia.util.ntp.enabled=true unless there is a firewall in place.");
			bFirstRunComplete = true;
		}
	}

	public static void startThread(){
		if (ntpt!=null)
			stopThread();

		ntpt = new NTPThread();
		ntpt.setDaemon(true);
		ntpt.start();
	}

	public static void stopThread(){
		try{
			((NTPThread)ntpt).stopIt();
		}
		catch (Exception e){
		}

		ntpt = null;
	}

	public static final long ntpOffset(){
		while (!bFirstRunComplete){
			try{
				Thread.sleep(10);
			}
			catch (Exception e){
			}
		}

		return lNTPOffset;
	}

	public NTPDate(){	// compensate for the GMT offset, return the new date in GMT time
		// make sure to call ntpOffset() BEFORE System.currentTimeMillis !!!!
		super( currentTimeMillis() );
	}

	public static final long currentTimeMillis(){
		return ntpOffset() + System.currentTimeMillis();
	}

	public String toString(){
		DateFormat formatter = null;

		formatter = new SimpleDateFormat("EEE MMM dd HH:mm:ss zzz yyyy", Locale.US);
		formatter.setTimeZone(TimeZone.getTimeZone("GMT"));

		return formatter.format(this);
	}

	public NTPDate(long l){
		super( ntpOffset() + l );
	}

}
