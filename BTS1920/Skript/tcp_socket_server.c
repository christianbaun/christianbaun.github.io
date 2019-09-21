// title:        tcp_socket_server.c
// description:  This c program is a very TCP socket example for Linux
// author:       Dr. Christian Baun
// url:          http://www.christianbaun.de
// license:      GPLv2
// date:         November 15th 2016
// version:      1.0
// gcc_version:  gcc (Debian 4.9.2-10) 4.9.2
// compile with: gcc tcp_socket_server.c -o tcp_socket_server
// nodes:        This program creates a socket and waits for an incomming 
//               connection from a client via TCP. The server waits for an 
//               incomming transmission and will reply a string to the client
//               The port number is specified as an argument when executing 
//               the compiled program:
//               ./tcp_socket_server <portnumber>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
    int sd;
    int neuer_socket;
    int portnummer;
    int clientadresselength;
    struct sockaddr_in adresse;

    char puffer[1024] = { 0 };
     
    // Wenn keine Portnummer als Argumument angegeben wurde...
    if (argc < 2) {
        printf("Sie müssen eine Portnummer angeben.\n");
        exit(1);
    }
     
    // Das erste Argument nach dem Datainamen ist die Portnummer
    portnummer = atoi(argv[1]);
     
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
    if (bind(sd, (struct sockaddr *) &adresse, sizeof(adresse)) < 0) {
        printf("Der Port ist nicht verfügbar.\n");
        exit(1);
    } else {
        printf("Der Socket wurde an eine Portnummer gebunden.\n");
    }

    // Eine Warteschlange für bis zu 5 eintreffende Verbindungsanforderung einrichten
    if (listen(sd, 5) == 0) {
        printf("Warte auf Verbindungsanforderungen.\n");
    } else {
        printf("Es kam beim listen zu einem Fehler.\n");
    }

    clientadresselength = sizeof(adresse);
    neuer_socket = accept(sd, (struct sockaddr *) &adresse, &clientadresselength);
    if (neuer_socket < 0) {
        printf("Verbindungsanforderung konnte nicht akzeptiert werden.\n");
        exit(1);
    } else {
        printf("Es wurde ein Verbindung zu einem Client hergestellt.\n");
    }
     
    // Inhalt des Puffers mit Null-Bytes füllen
    memset(puffer, 0, sizeof(puffer));

    // Nachricht empfangen
    if (read(neuer_socket, puffer, sizeof(puffer)) < 0) {
        printf("Der Lesezugriff ist fehlgeschlagen.\n");
        exit(1);
    }
     
    // Empfangene Nachricht lokal ausgeben
    printf("Empfangene Nachricht: %s\n",puffer);
     
    char antwort[]="Server: Ich habe Ihre Nachricht empfangen.\n";
    
    // Nachricht senden
    if (write(neuer_socket, antwort, sizeof(antwort)) < 0) {
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