// title:        fifo_beispiel.c
// description:  This c program is an example about inter-process communication 
//               via a named pipe (FIFO) for Linux
// author:       Dr. Christian Baun
// url:          http://www.christianbaun.de
// license:      GPLv2
// date:         June 1st 2021
// version:      1.2
// gcc_version:  gcc (Debian 8.3.0-6)
// compile with: gcc fifo_beispiel.c -o fifo_beispiel 
// nodes:        This program creates a child process and inside the parent 
//               process a named pipe (FIFO) is created. The parent process 
//               writes a string into the pipe and the child process reads the 
//               string out of the pipe

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

void main() {
  int pid_des_Kindes;
  
  // Die Pipe anlegen
  if (mkfifo("testfifo",0666) < 0) {
    printf("Das Anlegen der Pipe ist fehlgeschlagen.\n");
    // Programmabbruch
    exit(1);
  } else {
    printf("Die Pipe testfifo wurde angelegt.\n");
  }
  
  // Einen Kindprozess erzeugen
  pid_des_Kindes = fork();
  
  // Es kam beim fork zu einem Fehler
  if (pid_des_Kindes < 0) {
    perror("Es kam bei fork zu einem Fehler!\n");
    // Programmabbruch
    exit(1);
  }

  // Elternprozess
  if (pid_des_Kindes > 0) {    
    printf("Elternprozess: PID: %i\n", getpid());
    
    // Zugriffskennung der Pipe anlegen
    int fd;  
    char nachricht[] = "Testnachricht";
    
    // Die Pipe für Schreibzugriffe öffnen
    fd = open("testfifo", O_WRONLY);
    
    // Daten in die Pipe schreiben
    write(fd, &nachricht, sizeof(nachricht));
    
    // Die Pipe schließen
    close(fd);
    
    exit(0);
  }

  // Kindprozess
  if (pid_des_Kindes == 0) {
    printf("Kindprozess: PID: %i\n", getpid());

    // Zugriffskennung der Pipe anlegen
    int fd; 
    // Einen Empfangspuffer mit 80 Zeichen Kapazität anlegen
    char puffer[80];

    // Die Pipe für Lesezugriffe öffnen
    fd = open("testfifo", O_RDONLY);
    
    // Daten aus der Pipe auslesen 
    if (read(fd, puffer, sizeof(puffer)) > 0) {
      // Empfangene Daten ausgeben
      printf("Empfangene Daten: %s\n", puffer);
    } else {
      printf("Es kam beim Auslesen zu einem Fehler.\n");  
    }
    
    // Die Pipe schließen
    close(fd);
    
    // Die Pipe löschen
    if (unlink("testfifo") < 0) {
      printf("Das Löschen der Pipe ist fehlgeschlagen.\n");
      // Programmabbruch
      exit(1);
    } else {
      printf("Die Pipe wurde gelöscht.\n");
    }
  
    exit(0);
  }
}
