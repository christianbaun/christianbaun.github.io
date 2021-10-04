// title:        semaphore_beispiel_systemv.c
// description:  This c program is a simple semaphore (System V) example for Linux
// author:       Dr. Christian Baun
// url:          http://www.christianbaun.de
// license:      GPLv2
// date:         October 4th 2021
// version:      1.0
// gcc_version:  gcc 10.2.1 (Debian 10.2.1-6)
// compile with: gcc semaphore_beispiel_systemv.c -o semaphore_beispiel_systemv
// nodes:        This program creates a child process. The parent process and
//               the child process both try to print characters in the command
//               line interface (critical section). Each process may print 
//               only one character at a time. Two System V semaphores are used  
//               to ensure mutual exclusion.
//               System V semaphores can be monitored with command ipcs -s

#include <stdio.h>      // für printf
#include <stdlib.h>     // für exit
#include <unistd.h>     // für read, write, close
#include <sys/wait.h>   // für wait
#include <sys/sem.h>    // für semget, semctl, semop

void main() {
  int pid_des_kindes;
  int sem_key1=12345;
  int sem_key2=54321;
  int returncode_semget1, returncode_semget2;
  int returncode_semctl;
  int output;

  // Das Puffern Standardausgabe (stdout) unterbinden
  setbuf(stdout, NULL);

  // Neue Semaphorgruppe 12345 mit einer Semaphore erstellen 
  // IPC_CREAT = Semaphore erzeugen, wenn Sie noch nicht existiert
  // IPC_EXCL = Neuen Semaphorgruppe anlegen und nicht auf evtl. 
  // existierende Semaphorgruppe mit gleichem Schlüssel zugreifen
  // 0600 = Zugriffsrechte auf die neue Semaphorgruppe
  returncode_semget1 = semget(sem_key1, 1, IPC_CREAT | IPC_EXCL | 0600);
  if (returncode_semget1 < 0) {
    printf("Die Semaphorgruppe %i konnte nicht erstellt werden.\n", sem_key1);
    perror("semget");
    // Programmabbruch
    exit(1);
  }

  // Neue Semaphorgruppe 54321 mit einer Semaphore erstellen
  returncode_semget2 = semget(sem_key2, 1, IPC_CREAT | IPC_EXCL | 0600);
  if (returncode_semget2 < 0) {
    printf("Die Semaphorgruppe %i konnte nicht erstellt werden.\n", sem_key2);
    perror("semget");
    // Programmabbruch
    exit(1);
  }

  // P-Operation definieren. Wert der Zählveriable um eins dekrementieren 
  struct sembuf p_operation = {0, -1, 0};  
  
  // V-Operation definieren. Wert der Zählveriable um eins inkrementieren 
  struct sembuf v_operation = {0, 1, 0};    

  // Erste Semaphore der Semaphorgruppe 12345 initial auf Wert 1 setzen
  returncode_semctl = semctl(returncode_semget1, 0, SETVAL, 1);
  if (returncode_semctl < 0) {
    printf("Der Wert der Semaphore %i konnte nicht gesetzt werden.\n", sem_key1);
    perror ("semctl SETVAL"); 
    exit (1);
  }

  // Erste Semaphore der Semaphorgruppe 54321 initial auf Wert 0 setzen
  returncode_semctl = semctl(returncode_semget2, 0, SETVAL, 0);
  if (returncode_semctl < 0) {
    printf("Der Wert der Semaphore %i konnte nicht gesetzt werden.\n", sem_key2);
    perror ("semctl SETVAL"); 
    exit (1);
  }

  // Initialen Wert der ersten Semaphore der Semaphorgruppe 12345 zur Kontrolle ausgeben
  output = semctl(returncode_semget1, 0, GETVAL, 0);
  printf("Wert der Semaphore mit ID %i und Key %i: %i\n", returncode_semget1, sem_key1, output);

  // Initialen Wert der ersten Semaphore der Semaphorgruppe 54321 zur Kontrolle ausgeben
  output = semctl(returncode_semget2, 0, GETVAL, 0);
  printf("Wert der Semaphore mit ID %i und Key %i: %i\n", returncode_semget2, sem_key2, output);

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
      semop(returncode_semget2, &p_operation, 1); // P-Operation Semaphore 54321
      // Kritischer Abschnitt (Anfang)
      printf("B");
      sleep(1);
      // Kritischer Abschnitt (Ende)
      semop(returncode_semget1, &v_operation, 1); // V-Operation Semaphore 12345
    }
    exit(0);
  }
  
  // Elternprozess
  if (pid_des_kindes > 0) {       
    for (int i=0;i<5;i++) {
      semop(returncode_semget1, &p_operation, 1); // P-Operation Semaphore 12345
      // Kritischer Abschnitt (Anfang)
      printf("A");
      sleep(1);
      // Kritischer Abschnitt (Ende)
      semop(returncode_semget2, &v_operation, 1); // V-Operation Semaphore 54321
    }
  }

  // Warten auf die Beendigung des Kindprozesses
  wait(NULL);

  printf("\n");

  // Semaphorgruppe 12345 löschen
  returncode_semctl = semctl(returncode_semget1, 0, IPC_RMID, 0);
    if (returncode_semctl < 0) {
      printf("Die Semaphorgruppe %i konnte nicht gelöscht werden.\n", returncode_semget1);
      exit(1);
  } else { 
      printf("Die Semaphorgruppe mit ID %i und Key %i wurde gelöscht.\n", returncode_semget1, sem_key1);
  }    

  // Semaphorgruppe 54321 löschen
  returncode_semctl = semctl(returncode_semget2, 0, IPC_RMID, 0);
    if (returncode_semctl < 0) {
      printf("Die Semaphorgruppe %i konnte nicht gelöscht werden.\n", returncode_semget2);
      exit(1);
  } else { 
      printf("Die Semaphorgruppe mit ID %i und Key %i wurde gelöscht.\n", returncode_semget2, sem_key2);
  }

  exit(0);
}
