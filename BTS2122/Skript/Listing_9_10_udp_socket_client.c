// title:        Listing_9_10_udp_socket_client.c
// description:  This c program is a UDP socket example for Linux
// author:       Dr. Christian Baun
// url:          http://www.christianbaun.de
// license:      GPLv2
// date:         October 9th 2021
// version:      1.1
// gcc_version:  gcc 10.2.1 (Debian 10.2.1-6)
// compile with: gcc Listing_9_10_udp_socket_client.c -o Listing_9_10_udp_socket_client
// nodes:        This program connects with the socket of a server and transmits
//               a message which is typed in at the command line interface by the
//               user. The IP address and port number are specified as arguments on the
//               command line when executing the compiled program:
//               ./Listing_9_10_udp_socket_client <ip-address> <portnumber>

#include <stdio.h>      // für printf
#include <stdlib.h>     // für atoi, exit
#include <string.h>     // für strlen
#include <sys/socket.h> // für socket
#include <netinet/in.h> // für die Struktur sockaddr_in
#include <unistd.h>     // für read, write, close
#include <arpa/inet.h>	// für htons, inet_addr

int main(int argc, char *argv[])
{
  int sd;
  int portnummer;
  int adresse_laenge;
  struct sockaddr_in adresse;
  
  // Inhalt des Puffers mit Null-Bytes füllen
  char puffer[1024] = { 0 };

  // Wenn IP-Adresse und Portnummer fehlen...
  if (argc < 3) {
    printf("IP-Adresse und/oder Portnummer fehlen.\n");
    exit(1);
  }
    
  // Das zweite Argument nach dem Dateinamen ist die Portnummer
  portnummer = atoi(argv[2]);

  // Socket erstellen
  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sd < 0) {
    printf("Der Socket konnte nicht erzeugt werden.\n");
    exit(1);
  } else {
    printf("Der Socket wurde erzeugt.\n");
  }
  
  // Speicherbereich der Struktur sockaddr_in mit Nullen füllen
  memset(&adresse, 0, sizeof(adresse)); 
  
  // Socket-Adresse in der Struktur sockaddr_in speichern
  adresse.sin_family = AF_INET;
  adresse.sin_port = htons(portnummer);
  adresse.sin_addr.s_addr = inet_addr(argv[1]); 
  
  printf("Bitte geben Sie die Nachricht ein: ");
      
  // Nachricht von der Kommandozeile einlesen
  fgets(puffer, sizeof(puffer), stdin);
  
  adresse_laenge = sizeof(adresse);

  // Nachricht senden
  if (sendto(sd, 
             (const char *)puffer, 
             strlen(puffer), 
             0, 
             (struct sockaddr *) &adresse, 
             adresse_laenge) < 0) {
    printf("Der Schreibzugriff ist fehlgeschlagen.\n");
    exit(1);
  }
  
  // Den Inhalt des Puffers wieder mit Null-Bytes füllen
  memset(puffer, 0, sizeof(puffer));

  // Nachricht empfangen und in der Shell ausgeben
  if (recvfrom(sd, 
               (char *)puffer, 
               sizeof(puffer), 
               0, 
               (struct sockaddr *) &adresse, 
               &adresse_laenge) < 0) {
    printf("Der Lesezugriff ist fehlgeschlagen.\n");
    exit(1);
  } else {
    printf("%s\n",puffer); 
  }
      
  // Socket schließen
  if (close(sd) == 0) {
    printf("Der Socket wurde geschlossen.\n");
  }
  
  exit(0);
}
