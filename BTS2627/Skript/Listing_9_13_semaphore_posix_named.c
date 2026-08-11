// title:        Listing_9_13_semaphore_posix_named.c
// description:  This c program is a simple named semaphore (POSIX) example 
//               for Linux
// author:       Dr. Christian Baun
// url:          http://www.christianbaun.de
// license:      GPLv2
// date:         October 9th 2021
// version:      1.1
// gcc_version:  gcc 10.2.1 (Debian 10.2.1-6)
// compile with: gcc Listing_9_13_semaphore_posix_named.c -o Listing_9_13_semaphore_posix_named -lpthread
// nodes:        This program creates a child process. The parent process and
//               the child process both try to print characters in the command
//               line interface (critical section). Each process may print 
//               only one character at a time. Two named POSIX semaphores are   
//               used to ensure mutual exclusion. 
//               Attention: ipcs does only show System V semaphores and not  
//               POSIX semaphores. 
//               Named POSIX semaphores are created in Linux in the folder 
//               /dev/shm with names of the form sem.<name>

#include <stdio.h>      // for printf
#include <stdlib.h>     // for exit
#include <unistd.h>     // for read, write, close
#include <sys/wait.h>   // for wait
#include <semaphore.h>  // for sem_init, sem_post, sem_wait, sem_destroy
#include <fcntl.h>      // for O_CREAT
  
void main() {
  const char sem1_name[] = "/mysem1";
  const char sem2_name[] = "/mysem2";
  int pid_des_kindes;
  int returncode_close, returncode_unlink;
  int output;
  
  sem_t *mutex_sem1, *mutex_sem2;

  // Das Puffern Standardausgabe (stdout) unterbinden
  setbuf(stdout, NULL);

  // Neue benannte Semaphore /mysem1 erstellen die den initialen Wert 1 hat
  mutex_sem1 = sem_open(sem1_name, O_CREAT, 0600, 1);
  if (mutex_sem1 == SEM_FAILED) {
    printf("Die Semaphore konnte nicht erstellt werden.\n");
    perror("sem_open");
    exit(1);            // Programmabbruch
  } else { 
    printf("Semaphore %s wurde erstellt.\n", sem1_name);
  }    

  // Neue benannte Semaphore /mysem2 erstellen die den initialen Wert 0 hat
  mutex_sem2 = sem_open(sem2_name, O_CREAT, 0600, 0);
  if (mutex_sem2 == SEM_FAILED) {
    printf("Die Semaphore konnte nicht erstellt werden.\n");
    perror("sem_open");
    exit(1);            // Programmabbruch
  } else { 
    printf("Semaphore %s wurde erstellt.\n", sem2_name);
  }    

  // Initialen Wert der Semaphore /mysem1 zur Kontrolle ausgeben
  sem_getvalue(mutex_sem1, &output);
  printf("Wert von %s: %i\n", sem1_name, output);

  // Initialen Wert der Semaphore /mysem2 zur Kontrolle ausgeben
  sem_getvalue(mutex_sem2, &output);
  printf("Wert von %s: %i\n", sem2_name, output);

  // Einen Kindprozess erzeugen
  pid_des_kindes = fork();
  
  // Es kam beim fork zu einem Fehler
  if (pid_des_kindes < 0) {
    perror("Es kam bei fork zu einem Fehler!\n");
    exit(1);            // Programmabbruch
  }

  // Kindprozess
  if (pid_des_kindes == 0) {
    for (int i=0;i<5;i++) {
      sem_wait(mutex_sem2);      // P-Operation Semaphore /mysem2
      // Kritischer Abschnitt (Anfang)
      // Pause. Zwischen 0 und 2 Sekunden warten
      sleep(rand() % 3);
      printf("B");
      // Kritischer Abschnitt (Ende)
      sem_post(mutex_sem1);      // V-Operation Semaphore /mysem1
    }
    
    exit(0);
  }
  
  // Elternprozess
  if (pid_des_kindes > 0) {       
    for (int i=0;i<5;i++) {
      sem_wait(mutex_sem1);      // P-Operation Semaphore /mysem1
      // Kritischer Abschnitt (Anfang)
      // Pause. Zwischen 0 und 2 Sekunden warten
      sleep(rand() % 3);
      printf("A");
      // Kritischer Abschnitt (Ende)
      sem_post(mutex_sem2);      // V-Operation Semaphore /mysem2
    }
  }

  // Warten auf die Beendigung des Kindprozesses
  wait(NULL);

  printf("\n");

  // Semaphore /mysem1 schliessen
  returncode_close = sem_close(mutex_sem1);
    if (returncode_close < 0) {
      printf("%s konnte nicht geschlossen werden.\n", sem1_name);
      exit(1);            // Programmabbruch
  } else { 
      printf("%s wurde geschlossen.\n", sem1_name);
  }    

  // Semaphore /mysem2 schliessen
  returncode_close = sem_close(mutex_sem2);
    if (returncode_close < 0) {
      printf("%s konnte nicht geschlossen werden.\n", sem2_name);
      exit(1);            // Programmabbruch
  } else { 
      printf("%s wurde geschlossen.\n", sem2_name);
  }
  
  // Semaphore /mysem1 entfernen
  returncode_unlink = sem_unlink(sem1_name);
    if (returncode_unlink < 0) {
      printf("%s konnte nicht entfernt werden.\n", sem1_name);
      exit(1);            // Programmabbruch
  } else { 
      printf("%s wurde entfernt.\n", sem1_name);
  }    

  // Semaphore /mysem2 entfernen
  returncode_unlink = sem_unlink(sem2_name);
    if (returncode_unlink < 0) {
      printf("%s konnte nicht entfernt werden.\n", sem2_name);
      exit(1);            // Programmabbruch
  } else { 
      printf("%s wurde entfernt.\n", sem2_name);
  }

  exit(0);
}
