// title:        Listing_9_12_semaphore_systemv.c
// description:  This c program is a simple semaphore (System V) example for Linux
// author:       Dr. Christian Baun
// url:          http://www.christianbaun.de
// license:      GPLv2
// date:         October 9th 2021
// version:      1.1
// gcc_version:  gcc 10.2.1 (Debian 10.2.1-6)
// compile with: gcc Listing_9_12_semaphore_systemv.c -o Listing_9_12_semaphore_systemv
// nodes:        This program creates a child process. The parent process and
//               the child process both try to print characters in the command
//               line interface (critical section). Each process may print 
//               only one character at a time. Two System V semaphores are used  
//               to ensure mutual exclusion.
//               System V semaphores can be monitored with command ipcs -s

#include <stdio.h>      // for printf
#include <stdlib.h>     // for exit
#include <unistd.h>     // for read, write, close
#include <sys/wait.h>   // for wait
#include <sys/sem.h>    // for semget, semctl, semop

void main() {
  int pid_des_kindes;
  int key1=12345;
  int key2=54321;
  int rc_semget1;       // Rückgabewert (return code) von semget
  int rc_semget2;       // Rückgabewert (return code) von semget
  int rc_semctl;        // Rückgabewert (return code) von semctl
  int output;

  // Das Puffern Standardausgabe (stdout) unterbinden
  setbuf(stdout, NULL);

  // Neue Semaphorgruppe 12345 mit einer Semaphore erstellen 
  // IPC_CREAT = Semaphore erzeugen, wenn Sie noch nicht existiert
  // IPC_EXCL = Neuen Semaphorgruppe anlegen und nicht auf evtl. 
  // existierende Semaphorgruppe mit gleichem Key zugreifen
  // 0600 = Zugriffsrechte auf die neue Semaphorgruppe
  rc_semget1 = semget(key1, 1, IPC_CREAT | IPC_EXCL | 0600);
  if (rc_semget1 < 0) {
    printf("Die Semaphorgruppe konnte nicht erstellt werden.\n");
    perror("semget");
    // Programmabbruch
    exit(1);
  } else { 
    printf("Semaphorgruppe %i mit Key %i wurde erstellt.\n",
           rc_semget1, key1);
  }    

  // Neue Semaphorgruppe 54321 mit einer Semaphore erstellen
  rc_semget2 = semget(key2, 1, IPC_CREAT | IPC_EXCL | 0600);
  if (rc_semget2 < 0) {
    printf("Die Semaphorgruppe konnte nicht erstellt werden.\n");
    perror("semget");
    // Programmabbruch
    exit(1);
  } else { 
    printf("Semaphorgruppe %i mit Key %i wurde erstellt.\n",
           rc_semget2, key2);
  }   

  // P-Operation definieren. Wert der Semaphore um eins dekrementieren 
  struct sembuf p_operation = {0, -1, 0};  
  
  // V-Operation definieren. Wert der Semaphore um eins inkrementieren 
  struct sembuf v_operation = {0, 1, 0};    

  // Erste Semaphore der Semaphorgruppe 12345 initial auf Wert 1 setzen
  rc_semctl = semctl(rc_semget1, 0, SETVAL, 1);
  if (rc_semctl < 0) {
    printf("Der Wert von %i konnte nicht gesetzt werden.\n", key1);
    perror ("semctl SETVAL"); 
    exit (1);
  }

  // Erste Semaphore der Semaphorgruppe 54321 initial auf Wert 0 setzen
  rc_semctl = semctl(rc_semget2, 0, SETVAL, 0);
  if (rc_semctl < 0) {
    printf("Der Wert von %i konnte nicht gesetzt werden.\n", key2);
    perror ("semctl SETVAL"); 
    exit (1);
  }

  // Initialen Wert der ersten Semaphore der Semaphorgruppe 12345 zur Kontrolle ausgeben
  output = semctl(rc_semget1, 0, GETVAL, 0);
  printf("Wert der Semaphorgruppe %i: %i\n", rc_semget1, output);

  // Initialen Wert der ersten Semaphore der Semaphorgruppe 54321 zur Kontrolle ausgeben
  output = semctl(rc_semget2, 0, GETVAL, 0);
  printf("Wert der Semaphorgruppe %i: %i\n", rc_semget2, output);

  // Einen Kindprozess erzeugen
  pid_des_kindes = fork();
  
  // Es kam beim fork zu einem Fehler
  if (pid_des_kindes < 0) {
    perror("Es kam bei fork zu einem Fehler!\n");
    // Programmabbruch
    exit(1);
  }

  // Kindprozess
  if (pid_des_kindes == 0) {
    for (int i=0;i<5;i++) {
      semop(rc_semget2, &p_operation, 1); // P-Operation Semaphore 54321
      // Kritischer Abschnitt (Anfang)
      // Pause. Zwischen 0 und 2 Sekunden warten
      sleep(rand() % 3);
      printf("B");
      // Kritischer Abschnitt (Ende)
      semop(rc_semget1, &v_operation, 1); // V-Operation Semaphore 12345
    }
    exit(0);
  }
  
  // Elternprozess
  if (pid_des_kindes > 0) {       
    for (int i=0;i<5;i++) {
      semop(rc_semget1, &p_operation, 1); // P-Operation Semaphore 12345
      // Kritischer Abschnitt (Anfang)
      // Pause. Zwischen 0 und 2 Sekunden warten
      sleep(rand() % 3);
      printf("A");
      // Kritischer Abschnitt (Ende)
      semop(rc_semget2, &v_operation, 1); // V-Operation Semaphore 54321
    }
  }

  // Warten auf die Beendigung des Kindprozesses
  wait(NULL);

  printf("\n");

  // Semaphorgruppe 12345 entfernen
  rc_semctl = semctl(rc_semget1, 0, IPC_RMID, 0);
    if (rc_semctl < 0) {
      printf("Semaphorgruppe konnte nicht entfernt werden.\n");
      perror("semctl");
      exit(1);
  } else { 
      printf("Semaphorgruppe %i wurde entfernt.\n", rc_semget1);
  }    

  // Semaphorgruppe 54321 entfernen
  rc_semctl = semctl(rc_semget2, 0, IPC_RMID, 0);
    if (rc_semctl < 0) {
      printf("Semaphorgruppe konnte nicht entfernt werden.\n");
      perror("semctl");
      exit(1);
  } else { 
      printf("Semaphorgruppe %i wurde entfernt.\n", rc_semget2);
  }

  exit(0);
}
