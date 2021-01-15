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

import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.LinkedList;

/**
 * Procesul raspunzator de o copie locala a memoriei partajate.
 *
 */
public class SharedMemoryProcess {

	// serverul local cu ajutorul caruia sunt primite updateuri
	private ServerSocket server;
	// adresa locala a serverului - numele sub care procesul este vazut in sistemul distribuit va fi adresa:port 
	public LocalAddress localAddress;
	// topologia curenta cunoscuta de procesul local
	private final LinkedList topology = new LinkedList();

	// replica locala procesului
	private SharedMemoryReplica replica;

	/** Valori de posibile mesaje de update */

	private static final Byte sendDiscovery = Byte.valueOf((byte)0);
	private static final Byte requestReplica = Byte.valueOf((byte)1);
	private static final Byte readVar = Byte.valueOf((byte)2);
	private static final Byte writeVar = Byte.valueOf((byte)3);
	private static final Byte removeProcess = Byte.valueOf((byte)4);
	private static final Byte newOwner = Byte.valueOf((byte)5);
	private static final Byte changeValue = Byte.valueOf((byte)6);

	private static final int maxTimeout = 10 * 1000;
	
	/**
	 * Constructorul primeste ca argumente o adresa si un port al primului peer din cadrul sistemului distribuit.
	 * @param discoveryAddress Adresa clientului cu care se incepe procesul de discovery. Valoare null inseamna ca e primul proces din sistem
	 * nu mai trebuie facuta nici o descoperire, clientul ruland pe cont propriu
	 * @param discoveryPort Portul pe care asculta respectivul proces
	 * @throws Exception In cazul in care nu s-a putut porni serverul local de exemplu
	 */
	public SharedMemoryProcess(String discoveryAddress, int discoveryPort) throws Exception {
		// initializam serverul
		try {
			if (discoveryAddress == null && discoveryPort > 0) // daca adresa e nula si portul valid atunci deschidem serverul local folosind portul indicat
				server = new ServerSocket(discoveryPort);
			else
				server = new ServerSocket(0);
			System.out.println("Server started: ("+InetAddress.getLocalHost().getHostAddress()+":"+server.getLocalPort()+")");
			(new Thread(new ServerThread())).start(); // preluarea conexiunilor se face intr-un thread separat
			localAddress = new LocalAddress(InetAddress.getLocalHost().getHostAddress(), server.getLocalPort());
		} catch (Throwable t) {
			t.printStackTrace();
			throw new Exception("Error in creating the server socket");
		}
		synchronized (topology) {
			// initializam lista de vecini
			if (discoveryAddress != null) {
				LinkedList yetToSolve = new LinkedList();
				// incepem procesul de descoperire pornind de la adresa indicata prin argumentele constructorului
				yetToSolve.addLast(new LocalAddress(discoveryAddress, discoveryPort));  
				while (true) {
					if (yetToSolve.size() == 0) break;
					LocalAddress nextAddress = (LocalAddress)yetToSolve.removeFirst();
					LinkedList tmpList = init(nextAddress.address, nextAddress.port); // apelam initializarea descoperirii cu o anumita adresa
					if (tmpList == null) continue;
					if (!topology.contains(nextAddress) && !nextAddress.equals(localAddress))
						topology.addLast(nextAddress);
					for (Iterator it = tmpList.iterator(); it.hasNext(); ) { // iteram prin topologia nou aflata
						LocalAddress adr = (LocalAddress)it.next();
						if (!topology.contains(adr) && !adr.equals(localAddress)) // pentru a vedea daca nu cumva avem un peer inca nedescoperit
							yetToSolve.addLast(adr);
					}
				}
			}
			if (!topology.contains(localAddress))
				topology.addLast(localAddress);
			// afisam tabela de procese descoperite
			System.out.println("Current topology:");
			printTopology();
		}
		// initializam copia locala a memoriei partajate
		if (discoveryAddress != null) {
			try {
				requestReplica(discoveryAddress, discoveryPort);
			} catch (Exception e) {
				System.err.println("Got exception requesting replica from "+discoveryAddress+":"+discoveryPort);
				replica = new SharedMemoryReplica();
			}
		} else {
			replica = new SharedMemoryReplica();
		}
		replica.process = this;
	}

	/**
	 * Metoda ce intoarce copia locala a memoriei partajate.
	 */
	public SharedMemoryReplica getReplica() {
		return replica;
	}

	/**
	 * Procesul de initializare a descoperii topologiei cunoscute de un anumit peer din sistemul distribuit.
	 * @param discoveryAddress Adresa clientului pe care il interogam.
	 * @param discoveryPort Portul pe care respectivul client asculta cereri de noi conexiuni. 
	 * @return Topologia curenta cunoscuta de respectivul proces sau lista vida daca apar erori.
	 */
	public LinkedList init(String discoveryAddress, int discoveryPort) {
		LinkedList tmpList = null;
		// connect to a another process to start discovering
		try {
			// conectare la adresa indicata de argumentele furnizate
			Socket s = new Socket(discoveryAddress, discoveryPort);
			s.setSoTimeout(maxTimeout);
			ObjectInputStream ois = new ObjectInputStream(s.getInputStream());
			ObjectOutputStream oos = new ObjectOutputStream(s.getOutputStream());
			// trimitem tipul de mesaj (descoperire)
			oos.writeObject(sendDiscovery);
			// si ne prezentam
			oos.writeObject(localAddress);
			// citim raspunsul - topologia curent cunoscuta de respectivul client
			tmpList = (LinkedList)ois.readObject();
			try { ois.close(); oos.close(); s.close(); } catch (Exception e) { }
		} catch (Throwable t) {
			System.err.println("Got exception in initiatiating discovery with "+discoveryAddress+":"+discoveryPort);
			t.printStackTrace();
		}
		return tmpList;
	}

	/**
	 * Metoda prin intermediul careia se realizeaza interogarea copiei curente a memoriei partajate cunoscuta de un anumit proces.
	 * @param discoveryAddress Clientul de la care se cere copia curenta a memoriei partajate de procesele distribuite
	 * @param discoveryPort Portul pe care respectivul client asculta cereri de noi conexiuni
	 * @throws Exception
	 */
	public void requestReplica(String discoveryAddress, int discoveryPort) throws Exception {
		Socket s = new Socket(discoveryAddress, discoveryPort); // conectare
		s.setSoTimeout(maxTimeout);
		ObjectInputStream ois = new ObjectInputStream(s.getInputStream());
		ObjectOutputStream oos = new ObjectOutputStream(s.getOutputStream());
		// scriem mesajul - cerere copie curenta a replicii
		oos.writeObject(requestReplica);
		// ni se raspunde inapoi cu valoarea curenta a replicii de memorie partajata
		replica = (SharedMemoryReplica)ois.readObject();
		try { ois.close(); oos.close(); s.close(); } catch (Exception e) { }
	}

	/**
	 * Metoda prin care, in cazul in care este depistat un proces ce a fost scos din configuratie, cerem ownership-ul asupra unei variabile
	 * curent detinute de respectivul proces
	 * @param var Variabila asupra careia cerem dreptul de detinere
	 */
	private void takeOwnerShip(SharedMemoryVariable var) {
		var.owner = localAddress; // ne declaram noul detinator
		synchronized (topology) {
			// informam toate procese despre schimbarea survenita
			LinkedList yetToSolve = new LinkedList();
			for (Iterator it = topology.iterator(); it.hasNext(); ) {
				LocalAddress a = (LocalAddress)it.next();
				if (a.equals(localAddress)) continue;
				yetToSolve.addLast(a);
			}
			// pentru fiecare proces cunoscut din topologia curenta trimitem un mesaj de informare despre noul owner
			while (true) {
				if (yetToSolve.size() == 0) break;
				LocalAddress nextAddress = (LocalAddress)yetToSolve.removeFirst();
				// send the topology change
				try {
					Socket s = new Socket(nextAddress.address, nextAddress.port);
					s.setSoTimeout(maxTimeout);
					ObjectInputStream ois = new ObjectInputStream(s.getInputStream());
					ObjectOutputStream oos = new ObjectOutputStream(s.getOutputStream());
					// tipul de mesaj
					oos.writeObject(newOwner);
					// si noua valoare a respectivei variabile
					oos.writeObject(var);
					LinkedList topo = (LinkedList)ois.readObject();
					for (Iterator it2 = topo.iterator(); it2.hasNext(); ) {
						LocalAddress a = (LocalAddress)it2.next();
						if (!topology.contains(a) && !localAddress.equals(a)) {
							topology.add(a);
							yetToSolve.addLast(a);
						}
					}
					try { ois.close(); oos.close(); s.close(); } catch (Exception e) { }
				} catch (Exception e) {
					System.err.println("Got exception communicating new ownership with "+nextAddress);
					e.printStackTrace();
					// the process at nextAddress also suffered a problem...
					// we generate new voting for the detected lost stations...
					processLost(nextAddress); // we discovered another station with problems...
				}
			}
		}
	}

	/**
	 * Metoda apelata in momentul in care se detecteaza ca un anumit proces a iesit din sistemul distribuit. 
	 * In cadrul metodei se realizeaza algoritmul de vot a unui nou proces care sa preia atributiile
	 * anterior detinute de procesul care a iesit (ownership asupra variabilelor anterior detinute de respectivul proces)
	 * @param processAddress Adresa procesului ce a iesit din sistem
	 */
	private void processLost(LocalAddress processAddress) {
		synchronized (topology) {
			// actualizam corespunzator tabela de topologie locala
			topology.remove(processAddress);
			// afisam informatii despre schimbarea de topologia si pentru utilizator
			printTopology();
			LinkedList yetToSolve = new LinkedList();
			for (Iterator it = topology.iterator(); it.hasNext(); ) {
				LocalAddress a = (LocalAddress)it.next();
				if (a.equals(localAddress)) continue;
				yetToSolve.addLast(a);
			}
			// pentru fiecare peer trimitem un mesaj de actualizare a topologiei
			while (true) {
				if (yetToSolve.size() == 0) break;
				LocalAddress nextAddress = (LocalAddress)yetToSolve.removeFirst();
				// send the topology change
				try {
					Socket s = new Socket(nextAddress.address, nextAddress.port);
					s.setSoTimeout(maxTimeout);
					ObjectInputStream ois = new ObjectInputStream(s.getInputStream());
					ObjectOutputStream oos = new ObjectOutputStream(s.getOutputStream());
					// trimitem tipul de mesaj (proces disparut)
					oos.writeObject(removeProcess);
					// trimitem adresa procesului disparut
					oos.writeObject(processAddress);
					// si propria adresa pentru a initia procesul de vot
					oos.writeObject(localAddress);
					LinkedList topo = (LinkedList)ois.readObject();
					for (Iterator it2 = topo.iterator(); it2.hasNext(); ) {
						LocalAddress a = (LocalAddress)it2.next();
						if (!topology.contains(a) && !processAddress.equals(a) && !localAddress.equals(a)) {
							topology.add(a);
							yetToSolve.addLast(a);
						}
					}
					LocalAddress a = (LocalAddress)ois.readObject();
					if (!a.equals(localAddress)) { 
						// daca am primit de la un proces un vot negativ inseamna ca exista un alt proces ce nu a fost de acord 
						// ca noi sa preluam ownershipul variabilelor detinute de vechiul proces
						return;
					}
					try { ois.close(); oos.close(); s.close(); } catch (Exception e) { }
				} catch (Exception e) {
					System.err.println("Got exception communicating process removal with "+nextAddress);
					e.printStackTrace();
					// procesul de la nextAddress a suferit de asemenea o problema ...
					// generam un nou proces de votare pentru respectiva statie ...
					processLost(nextAddress); 
				}
			}
		}
		// daca am ajuns pana in acest punct inseamna ca toate procesele au fost de acord cu votul nostru, deci suntem
		// designated owners ai variabilelor anterior detinute de procesul ce a iesit din sistem
		synchronized (replica.variables) {
			for (Enumeration en = replica.variables.keys(); en.hasMoreElements(); ) {
				String varName = (String)en.nextElement();
				SharedMemoryVariable smv = (SharedMemoryVariable)replica.variables.get(varName);
				if (smv.owner.equals(processAddress)) {
					// initiem campania de informare ca variabila ne apartine, toate interogarile viitoare trebuind sa treaca prin noi
					takeOwnerShip(smv);
				}
			}
		}
	}

	/**
	 * Metoda de citire a unei variabile.
	 * @param var Variabila de citit
	 * @return Valoarea curenta a respectivei variabile.
	 */
	public Object read(SharedMemoryVariable var) {
		if (var.owner.equals(localAddress)) // daca suntem owneri 
			return ((SharedMemoryVariable)replica.variables.get(var.name)).value;
		// altfel interogam procesul detinator al respectivei variabile
		try {
			Socket s = new Socket(var.owner.address, var.owner.port);
			s.setSoTimeout(maxTimeout);
			ObjectInputStream ois = new ObjectInputStream(s.getInputStream());
			ObjectOutputStream oos = new ObjectOutputStream(s.getOutputStream());
			// tipul de mesaj (citire valoare variabila)
			oos.writeObject(readVar);
			// numele variabilei
			oos.writeObject(var.name);
			// memoria partajata
			SharedMemoryVariable v = (SharedMemoryVariable)ois.readObject();
			// in acest punct exista posibilitatea sa existe un defazaj de actualizare a copiei de memorie partajata locala
			// si in realitate un alt proces intre timp sa fi devenit owner al variabilei
			if (!v.owner.equals(var.owner)) {
				// in acest caz interogam in continuare noul owner
				replica.variables.put(v.name, v);
				return read(v);
			}
			// altfel am obtinut valoarea curenta a variabilei
			var.value = (Serializable)v.value;
			try { ois.close(); oos.close(); s.close(); } catch (Exception e) { }
			return v.value;
		} catch (Throwable t) {
			System.err.println("Got exception reading with "+var.owner);
			t.printStackTrace();
		}
		// daca am ajuns aici inseamna ca a aparut o problema... 
		// owner-ul este scos din lista si procesul curent va incerca sa devina noul owner al respectivei variabile (initializarea procesului de vot) 
		processLost(var.owner);
		return read(var);
	}

	/**
	 * Metoda prin care se scrie continutul unei variabile.
	 * @param var Variabila a carei continut se doreste actualizat intre copiile memoriei partajate.
	 */
	public void write(SharedMemoryVariable var) {
		// daca variabila este detinuta chiar de procesul local
		if (var.owner.equals(localAddress)) {
			replica.variables.put(var.name, var);
			// incepem informarea tuturor proceselor asupra noii valori a variabilei 
			broadcastNewVar(var);
			return;
		}
		// altfel trimitem noua valoare procesului owner al respectivei variabile
		try {
			Socket s = new Socket(var.owner.address, var.owner.port);
			s.setSoTimeout(maxTimeout);
			ObjectInputStream ois = new ObjectInputStream(s.getInputStream());
			ObjectOutputStream oos = new ObjectOutputStream(s.getOutputStream());
			// tipul de mesaje (actualizare variabila)
			oos.writeObject(writeVar);
			// si noua valoare
			oos.writeObject(var);
			// ni se raspunde prin confirmarea valorii actualizate
			Object o = ois.readObject();
			var.value = (Serializable)o;
			try { ois.close(); oos.close(); s.close(); } catch (Exception e) { }
			return;
		} catch (Throwable t) {
			System.err.println("Got error writing with "+var.owner);
			t.printStackTrace();
		}
		// daca am ajuns aici inseamna ca a aparut o problema... 
		// owner-ul este scos din lista si procesul curent va incerca sa devina noul owner al respectivei variabile (initializarea procesului de vot) 
		replica.variables.put(var.name, var);
		processLost(var.owner);
	}

	/**
	 * Metoda prin care se anunta toate procesele din sistem asupra unei actualizari a valorii unei variabile... metoda este apelata
	 * intotdeauna de catre procesul owner.
	 * @param var Noua variabila
	 */
	private void broadcastNewVar(SharedMemoryVariable var) {
		synchronized (topology) {
			// informam toate procesele curent cunoscute de noua schimbare
			LinkedList yetToSolve = new LinkedList();
			for (Iterator it = topology.iterator(); it.hasNext(); ) {
				LocalAddress a = (LocalAddress)it.next();
				if (a.equals(localAddress)) continue;
				yetToSolve.addLast(a);
			}
			while (true) {
				if (yetToSolve.size() == 0) break;
				LocalAddress nextAddress = (LocalAddress)yetToSolve.removeFirst();
				// trimitem actualizarea memoriei partajate fiecarui proces
				try {
					Socket s = new Socket(nextAddress.address, nextAddress.port);
					s.setSoTimeout(maxTimeout);
					ObjectInputStream ois = new ObjectInputStream(s.getInputStream());
					ObjectOutputStream oos = new ObjectOutputStream(s.getOutputStream());
					// tipul de mesaj (actualizare variabila)
					oos.writeObject(changeValue);
					// si valoarea noii variabile
					oos.writeObject(var);
					// ni se raspunde cu topologia curent detinuta de respectivul proces
					// astfel incat daca apar defazaje de actualizare intre topologia locala curent cunoscuta
					// si cea cunoscuta de procesul remote sa putem totusi sa informam toate procesele
					LinkedList topo = (LinkedList)ois.readObject();
					for (Iterator it2 = topo.iterator(); it2.hasNext(); ) {
						LocalAddress a = (LocalAddress)it2.next();
						if (!topology.contains(a) && !localAddress.equals(a)) {
							topology.add(a);
							yetToSolve.addLast(a);
						}
					}
					try { ois.close(); oos.close(); s.close(); } catch (Exception e) { }
				} catch (Exception e) { 
					System.err.println("Got exception communiacting change of value with "+nextAddress);
					e.printStackTrace();
					// procesul de la nextAddress a suferit de asemenea o problema ...
					// generam un nou proces de votare pentru respectiva statie ...
					processLost(nextAddress); 
				}
			}
		}
	}

	/**
	 * Metoda ce implementeaza tratarea diverselor tipuri de mesaje primite de procesul curent.
	 * @param command Valoarea tipului de mesaj primit
	 * @param ois Stream-ul de date de intrare
	 * @param oos Stream-ul de date de iesire
	 * @throws Exception
	 */
	public void treatCommand(Byte command, ObjectInputStream ois, ObjectOutputStream oos) throws Exception {
		if (command == null) return;
		if (command.equals(sendDiscovery)) { // descoperire de topologie
			// citim adresa (identificatorul) procesul ce a intrat in retea
			LocalAddress adr = (LocalAddress)ois.readObject(); 
			// trimitem inapoi o copie a topologiei curent cunoscute...
			oos.writeObject(topology);
			synchronized (topology) {
				if (!topology.contains(adr))
					topology.addLast(adr);
				// si afisam noua schimbare de topologie
				printTopology();
			}
			return;
		}
		if (command.equals(requestReplica)) { // cererea unei copii a memoriei partajate curent cunoscute de proces
			oos.writeObject(replica); // trimitem inapoi copia
			return;
		}
		if (command.equals(readVar)) { // daca am primit cerere de citire continut variabila
			String varName = (String)ois.readObject(); // citim numele variabilei 
			oos.writeObject(replica.variables.get(varName)); // si trimitem valoarea curenta cunoscuta local
			return;
		}
		if (command.equals(writeVar)) { // daca am primit cerere de actualizare continut variabila
			SharedMemoryVariable var = (SharedMemoryVariable)ois.readObject(); // citim noua valoare
			SharedMemoryVariable localVar = (SharedMemoryVariable)replica.variables.get(var.name);
			if (localVar != null && var.owner != localVar.owner) { // comparam cu valoarea curent cunoscuta de proces
				localVar.value = var.value; // new value
				write(localVar); // daca owner-ul e altul, s-a produs un defasaj si informam owner-ul corect asupra actulizarii de continut
				oos.writeObject(localVar.value);
			} else { // altfel actualiam continutul curent
				replica.variables.put(var.name, var);
				oos.writeObject(var.value);
				broadcastNewVar(var); // si informam toate procesele asupra actualizarii
			}
			return;
		}
		if (command.equals(removeProcess)) { // daca am primit un mesaj de inlaturare a unui proces 
			LocalAddress lostAddress = (LocalAddress)ois.readObject(); // citim adresa statiei disparute
			LocalAddress requestAddress = (LocalAddress)ois.readObject(); // si pe cea a statiei ce vrea sa devina owner
			synchronized (topology) {
				topology.remove(lostAddress); // actualima tabela de topologie
				oos.writeObject(topology); // trimitem propria topologie inapoi
			}
			// si trimitem valoarea propriului nostru vor
			if (localAddress.toString().compareTo(requestAddress.toString()) < 0) { // acordam votul procesului remote 
				oos.writeObject(requestAddress);
			} else { // altfel votam pentru preluarea de catre procesul local a ownership-ului variabilelor anterior detinute de procesul disparut
				oos.writeObject(localAddress);
				// initiem propriul proces de votare
				processLost(lostAddress);
			}
			return;
		}
		if (command.equals(newOwner) || command.equals(changeValue)) { // daca am primit o comanda de actualizare a unei variabile
			SharedMemoryVariable var = (SharedMemoryVariable)ois.readObject(); // citim valoarea actualizam
			replica.variables.put(var.name, var); // actualizam copia locala a memoriei partajate
			oos.writeObject(topology); // si trimitem inapoi propria tabela de topologie
			return;
		}
	}

	/**
	 * Functionalitatea de preluare a conexiunilor serverului locala este tratata intr-un thread separat.
	 */
	private class ServerThread implements Runnable {
		public void run() {		
			while (true) {
				try {
					Socket s = server.accept();
					s.setSoTimeout(maxTimeout);
					ObjectOutputStream oos = new ObjectOutputStream(s.getOutputStream());
					ObjectInputStream ois = new ObjectInputStream(s.getInputStream());
					// fiecare noua conexiune cu un client remote este si ea la randul ei tratata intr-un thread separat
					(new Thread(new SocketThread(s, ois, oos))).start();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
	}

	/**
	 * Functionalitatea de tratare a conexiunii cu un client remote intr-un thread separat.
	 */
	private class SocketThread implements Runnable {
		private Socket s;
		private ObjectInputStream ois;
		private ObjectOutputStream oos;
		public SocketThread(Socket s, ObjectInputStream ois, ObjectOutputStream oos) {
			this.s = s;
			this.ois = ois;
			this.oos = oos;
		}
		public void run() {
			Thread.currentThread().setName(s.toString());
			while (true) {
				try {
					Byte command = (Byte)ois.readObject(); // citim comanda
					treatCommand(command, ois, oos); // si o tratam corespunzator
				} catch (Exception e) {
					break;
				}
			}
			try {
				ois.close();
				oos.close();
				s.close();
			} catch (Throwable t) { }
		}
	}

	/**
	 * Metoda ce ajuta la tiparirea tabelei de topologie curent cunoscuta de catre procesul curent.
	 */
	private void printTopology() {
		StringBuffer buf = new StringBuffer();
		for (Iterator it = topology.iterator(); it.hasNext(); ) {
			LocalAddress a = (LocalAddress)it.next();
			buf.append(a.toString()).append("\n");
		}
		System.out.println(buf.toString());
	}

} // end of class SharedMemoryProcess

