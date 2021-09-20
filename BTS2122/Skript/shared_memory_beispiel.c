// title:        shared_mememory_beispiel.c
// description:  This c program is an example about inter-process communication
//               via shared memory for Linux
// author:       Dr. Christian Baun
// url:          http://www.christianbaun.de
// license:      GPLv2
// date:         September 20th 2021
// version:      1.4
// gcc_version:  gcc (Debian 8.3.0-6)
// compile with: gcc shared_mememory_beispiel.c -o shared_mememory_beispiel
// nodes:        This program creates a shared memory segment, attaches it to
//               the running process, inserts a string into it, reads the 
//               string out of it, detaches it and finally erases it.
//               Check the status of existing shared memory segment with the 
//               command ipcs

#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#define MAXMEMSIZE 20

int main(int argc, char **argv) {
  int shared_memory_key = 12345;
  int returncode_shmget;
  int returncode_shmdt;
  int returncode_sprintf;
  int returncode_printf;
  int returncode_shmctl;
  char *sharedmempointer;
  
  // Gemeinsames Speichersegment erzeugen
  // IPC_CREAT = Segment erzeugen, wenn es noch nicht existiert
  // 0600 = Zugriffsrechte auf das neue Segment
  returncode_shmget = shmget(shared_memory_key, MAXMEMSIZE, IPC_CREAT | 0600);
  if (returncode_shmget < 0) {
    printf("Das Segment konnte nicht erstellt werden.\n");
    perror("shmget");
    exit(1);
  }
  
  printf("Das Segment wurde erstellt.\n");
  
  // Gemeinsames Speichersegment anhängen
  sharedmempointer = shmat(returncode_shmget, 0, 0);
  if (sharedmempointer==(char *)-1) {
    printf("Das Segment konnte nicht angehängt werden.\n");
    perror("shmat");
    exit(1);
  } else { 
    printf("Das Segment wurde angehängt.\n");
  }
  
  // Eine Zeichenkette in das gemeinsame Speichersegment schreiben. 
  returncode_sprintf = sprintf(sharedmempointer, "Hallo Welt."); 
  if (returncode_sprintf < 0) {
    printf("Der Schreibzugriff ist fehlgeschlagen.\n");
    exit(1);
  } else { 
    printf("%i Zeichen wurden in das Segment geschrieben.\n", returncode_sprintf);
  }
  
  // Die Zeichenkette im gemeinsamen Speichersegment ausgeben.
  if (printf ("Inhalt des Segments: %s\n", sharedmempointer) < 0) {
    printf("Der Lesezugriff ist fehlgeschlagen.\n");
    exit(1);
  }
  
  // Gemeinsames Speichersegment lösen
  returncode_shmdt = shmdt(sharedmempointer);
  if (returncode_shmdt < 0) {
    printf("Das Segment konnte nicht gelöst werden.\n");
    perror("shmdt");
    exit(1);
  } else { 
    printf("Das Segment wurde vom Prozess gelöst.\n");
  }
  
  // Gemeinsames Speichersegment löschen
  returncode_shmctl = shmctl(returncode_shmget, IPC_RMID, 0);
  if (returncode_shmctl == -1) {
    printf("Das Segment konnte nicht gelöscht werden.\n");
    perror("shmctl");
    exit(1);
  } else { 
    printf("Das Segment wurde gelöscht.\n");
  }
    
  exit(0);
}
