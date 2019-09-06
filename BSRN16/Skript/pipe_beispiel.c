// title:        pipe_beispiel.c
// description:  This c program is a very simple example about inter-process communcation via a pipe for Linux
// author:       Dr. Christian Baun
// url:          http://www.christianbaun.de
// license:      GPLv2
// date:         November 11th 2016
// version:      1.0
// gcc_version:  gcc (Debian 4.9.2-10) 4.9.2
// compile with: gcc fifo_beispiel.c -o fifo_beispiel 
// nodes:        This program creates a child process and inside the parent 
//               process a pipe is created. The parent process writes a string
//               into the pipe and the child process reads the string out of
//               the pipe

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void main() {
  int pid_des_Kindes;
  // Zugriffskennungen der Pipe zum Lesen (testpipe[0]) und Schreiben (testpipe[01]) anlegen
  int testpipe[2]; 
  
  // Die Pipe testpipe anlegen
  if (pipe(testpipe) < 0) {
    printf("Das Anlegen der Pipe ist fehlgeschlagen.\n");
    // Programmabbruch
    exit(1);
  } else {
    printf("Die Pipe testpipe wurde angelegt.\n");
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
    
    // Lesekanal der Pipe testpipe blockieren
    close(testpipe[0]);
        
    char nachricht[] = "Testnachricht";
    
    // Daten in den Schreibkanal der Pipe schreiben
    write(testpipe[1], &nachricht, sizeof(nachricht));
  }

  // Kindprozess
  if (pid_des_Kindes == 0) {
    printf("Kindprozess: PID: %i\n", getpid());
    
    // Schreibkanal der Pipe testpipe blockieren
    close(testpipe[1]);
    
    // Einen Empfangspuffer mit 80 Zeichen Kapaiztät anlegen
    char puffer[80];
    
    // Daten aus dem Lesekanal der Pipe auslesen
    read(testpipe[0], puffer, sizeof(puffer));
    
    // Empfangene Daten ausgeben
    printf("Empfangene Daten: %s\n", puffer);
  }
}