// title:        tcp_socket_client.c
// description:  This c program is a very TCP socket example for Linux
// author:       Dr. Christian Baun
// url:          http://www.christianbaun.de
// license:      GPLv2
// date:         June 1st 2021
// version:      1.2
// gcc_version:  gcc (Debian 8.3.0-6)
// compile with: gcc tcp_socket_client.c -o tcp_socket_client
// nodes:        This program connects with the socket of a server and transmits
//               a message which is typed in at the command line interface by the
//               user. The IP address and portnumber are specified as arguments on the
//               command line when executing the compiled program:
//               ./tcp_socket_client <ip-address> <portnumber>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int sd;
    int portnummer;
    struct sockaddr_in adresse;
    struct hostent *server;
    
    char puffer[1024] = { 0 };

    // Wenn Hostname und Portnummer als Argumumente fehlen...
    if (argc < 3) {
        printf("Sie müssen einen Hostnamen und eine Portnummer angeben.\n");
        exit(1);
    }
     
    // Das zweite Argument nach dem Datainamen ist die Portnummer
    portnummer = atoi(argv[2]);

    // Socket erstellen
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        printf("Der Socket konnte nicht erzeugt werden.\n");
        exit(1);
    } else {
        printf("Der Socket wurde erzeugt.\n");
    }
     
    // Die IP-Adresse zum Hostnamen erhalten
    server = gethostbyname(argv[1]);
    if (server == 0) {
        printf("Der Server ist nicht erreichbar.\n");
        exit(1);
    }
    
    // Socket-Adresse in der Struktur sockaddr_in speichern
    adresse.sin_family = AF_INET;
    inet_aton (argv[1], &adresse.sin_addr.s_addr);
    adresse.sin_port = htons(portnummer);
    
    // Verbindungsanfrage an Server senden
    if (connect(sd, (struct sockaddr *) &adresse, sizeof(adresse)) < 0)  {
        printf("Die Verbindungsanfrage ist gescheitert.\n");
        exit(1);
    } else {
        printf("Die Verbindung zum Server ist hergestellt.\n");
    }
    
    printf("Bitte geben Sie die Nachricht ein: ");
    
    // Inhalt des Puffers mit Null-Bytes füllen
    memset(puffer, 0, sizeof(puffer));
    
    // Nachricht von der Kommandozeile einlesen
    fgets(puffer, sizeof(puffer), stdin);
    
    // Nachricht senden
    if (write(sd, puffer, strlen(puffer)) < 0) {
        printf("Der Schreibzugriff ist fehlgeschlagen.\n");
        exit(1);
    }
    
    // Inhalt des Puffers mit Null-Bytes füllen
    memset(puffer, 0, sizeof(puffer));
    
    // Nachricht empfangen
    if (read(sd, puffer, sizeof(puffer)) < 0) {
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
