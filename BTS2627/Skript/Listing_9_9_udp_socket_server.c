// title:        Listing_9_9_udp_socket_server.c
// description:  This c program is a UDP socket example for Linux
// author:       Dr. Christian Baun
// url:          http://www.christianbaun.de
// license:      GPLv2
// date:         October 9th 2021
// version:      1.1
// gcc_version:  gcc 10.2.1 (Debian 10.2.1-6)
// compile with: gcc Listing_9_9_udp_socket_server.c -o Listing_9_9_udp_socket_server
// nodes:        This program creates a socket and waits for an incoming 
//               connection from a client via UDP. The server waits for an 
//               incoming transmission and will reply a string to the client
//               The port number is specified as an argument when executing 
//               the compiled program:
//               ./Listing_9_9_udp_socket_server <portnumber>

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
  int portnummer;
  int client_adresse_laenge;
  struct sockaddr_in adresse, client_adresse;

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
  // Speicherbereich der Struktur sockaddr_in mit Nullen füllen
  memset(&client_adresse, 0, sizeof(client_adresse)); 
      
  // Socket-Adresse in der Struktur sockaddr_in speichern
  adresse.sin_family = AF_INET;
  adresse.sin_addr.s_addr = INADDR_ANY;
  adresse.sin_port = htons(portnummer);
    
  // Socket erstellen
  sd = socket(AF_INET, SOCK_DGRAM, 0);
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
    
  client_adresse_laenge = sizeof(client_adresse);

  // Nachricht empfangen
  if (recvfrom(sd, 
               (char *)puffer, 
               sizeof(puffer), 
               0, 
               (struct sockaddr *) &client_adresse,
               &client_adresse_laenge) < 0) {
    printf("Der Lesezugriff ist fehlgeschlagen.\n");
    exit(1);
  }
    
  // Empfangene Nachricht lokal ausgeben
  printf("Empfangene Nachricht: %s\n",puffer);
  
  char antwort[]="Server: Nachricht empfangen.\n";
  
  // Nachricht senden
  if (sendto(sd, 
             (const char *)antwort, 
             sizeof(antwort), 
             0, 
             (struct sockaddr *) &client_adresse,
             client_adresse_laenge) < 0) {
    printf("Der Schreibzugriff ist fehlgeschlagen.\n");
    exit(1);
  }

  // Socket schließen
  if (close(sd) == 0) {
    printf("Der Socket wurde geschlossen.\n");
  }
  
  exit(0);
}
