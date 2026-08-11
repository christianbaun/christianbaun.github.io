// title:        Listing_9_7_tcp_socket_server.c
// description:  This c program is a TCP socket example for Linux
// author:       Dr. Christian Baun
// url:          http://www.christianbaun.de
// license:      GPLv2
// date:         October 9th 2021
// version:      1.4
// gcc_version:  gcc 10.2.1 (Debian 10.2.1-6)
// compile with: gcc Listing_9_7_tcp_socket_server.c -o Listing_9_7_tcp_socket_server
// nodes:        This program creates a socket and waits for an incoming 
//               connection from a client via TCP. The server waits for an 
//               incoming transmission and will reply a string to the client
//               The port number is specified as an argument when executing 
//               the compiled program:
//               ./Listing_9_7_tcp_socket_server <portnumber>

#include <stdio.h>      // für printf
#include <stdlib.h>     // für atoi, exit
#include <string.h>     // für memset
#include <sys/socket.h> // für socket
#include <netinet/in.h> // für die Struktur sockaddr_in
#include <unistd.h>     // für read, write, close
#include <arpa/inet.h>	// für htons

int main(int argc, char *argv[])
{
  int sd;
  int neuer_socket;
  int portnummer;
  int clientadresselength;
  struct sockaddr_in adresse;

  // Inhalt des Puffers mit Null-Bytes füllen
  char puffer[1024] = { 0 };
    
  // Die Portnummer muss als Argument angegeben sein
  if (argc < 2) {
    printf("Sie müssen eine Portnummer angeben.\n");
    exit(1);
  }
    
  // Das Argument nach dem Dateinamen ist die Portnummer
  portnummer = atoi(argv[1]);
  
  // Speicherbereich der Struktur sockaddr_in mit Nullen füllen
  memset(&adresse, 0, sizeof(adresse)); 
    
  // Socket-Adresse in der Struktur sockaddr_in speichern
  adresse.sin_family = AF_INET;
  adresse.sin_addr.s_addr = INADDR_ANY;
  adresse.sin_port = htons(portnummer);
    
  // Socket erstellen
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0) {
    printf("Der Socket konnte nicht erzeugt werden.\n");
    exit(1);
  } else {
    printf("Der Socket wurde erzeugt.\n");
  }

  // Socket an eine Portnummer binden
  if (bind(sd, 
           (struct sockaddr *) &adresse, 
           sizeof(adresse)) < 0) {
    printf("Der Port ist nicht verfügbar.\n");
    exit(1);
  } else {
    printf("Der Socket wurde an den Port gebunden.\n");
  }

  // Eine Warteschlange für bis zu 5  Verbindungsanforderungen einrichten
  if (listen(sd, 5) == 0) {
    printf("Warte auf Verbindungsanforderungen.\n");
  } else {
    printf("Es kam beim listen zu einem Fehler.\n");
    exit(1);
  }

  clientadresselength = sizeof(adresse);
  neuer_socket = accept(sd, 
                        (struct sockaddr *) &adresse, 
                        &clientadresselength);
  if (neuer_socket < 0) {
    printf("Verbindungsanforderung fehlgeschlagen.\n");
    exit(1);
  } else {
    printf("Verbindung zu einem Client aufgebaut.\n");
  }

  // Nachricht empfangen
  if (read(neuer_socket, puffer, sizeof(puffer)) < 0) {
    printf("Der Lesezugriff ist fehlgeschlagen.\n");
    exit(1);
  }
    
  // Empfangene Nachricht lokal ausgeben
  printf("Empfangene Nachricht: %s\n",puffer);
    
  char antwort[]="Server: Nachricht empfangen.\n";
  
  // Nachricht senden
  if (write(neuer_socket, 
            antwort, 
            sizeof(antwort)) < 0) {
    printf("Der Schreibzugriff ist fehlgeschlagen.\n");
    exit(1);
  }

  // Socket schließen
  if (close(neuer_socket) == 0) {
    printf("Der verbundene Socket wurde geschlossen.\n");
  }

  // Socket schließen
  if (close(sd) == 0) {
    printf("Der Socket wurde geschlossen.\n");
  }
  
  exit(0);
}
