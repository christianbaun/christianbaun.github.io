// title:        Listing_9_2_shared_memory_posix.c
// description:  This c program is an example about inter-process communication
//               via shared memory (POSIX) for Linux
// author:       Dr. Christian Baun
// url:          http://www.christianbaun.de
// license:      GPLv2
// date:         October 9th 2021
// version:      1.5
// gcc_version:  gcc 10.2.1 (Debian 10.2.1-6)
// compile with: gcc Listing_9_2_shared_memory_posix.c -o Listing_9_2_shared_memory_posix -lrt
// nodes:        This program creates a POSIX shared memory segment, 
//               attaches it to the running process, inserts a string into it,
//               reads the string out of it, detaches it and finally erases it.
//               POSIX shared memory segments are created in Linux in the folder 
//               /dev/shm

#include <sys/mman.h>   // for shm_open, mmap, munmap, shm_unlink
#include <fcntl.h>      // for O_CREAT, O_CREAT
#include <stdio.h>      // for printf
#include <stdlib.h>     // for exit
#include <string.h>     // for strcpy
#include <unistd.h>
#define MAXMEMSIZE 20

int main(int argc, char **argv) {
  int shmem_fd;         // File Descriptor des Segments
  int rc_sprintf;       // Rückgabewert von sprintf
  void* shmem_pointer;
  const char name[] = "/mymemory";
  char nachricht[] = "Hallo Welt.";

  // Gemeinsames Speichersegment erzeugen
  // IPC_CREAT = Segment erzeugen, wenn es noch nicht existiert
  // O_RDWR = Segment lesend und schreibend öffnen
  // 0600 = Zugriffsrechte auf das neue Segment
  shmem_fd = shm_open(name, O_CREAT | O_RDWR, 0600);
  if (shmem_fd < 0) {
    printf("Das Speichersegment konnte nicht erstellt werden.\n");
    perror("shm_open");
    exit(1);
  } else {
    printf("Speichersegment %s wurde erstellt.\n", name);
  }

  // Größe des Speichersegments anpassen
  if (ftruncate(shmem_fd, MAXMEMSIZE) < 0) {
    printf("Die Größe konnte nicht angepasst werden.\n");
    perror("ftruncate");
    exit(1);
  } else { 
    printf("Die Größe von %s wurde angepasst.\n", name);
  }
  
  // Gemeinsames Speichersegment anhängen
  shmem_pointer = mmap(0, 
                       MAXMEMSIZE, 
                       PROT_WRITE, 
                       MAP_SHARED, 
                       shmem_fd, 
                       0);
  if (shmem_pointer < 0) {
    printf("%s konnte nicht angehängt werden.\n", name);
    perror("mmap");
    exit(1);
  } else {
    printf("%s wurde angehängt.\n", name);
  }
  
  // Eine Zeichenkette in das gemeinsame Speichersegment schreiben. 
  rc_sprintf = sprintf(shmem_pointer, "Hallo Welt."); 
  if (rc_sprintf < 0) {
    printf("Der Schreibzugriff ist fehlgeschlagen.\n");
    exit(1);
  } else { 
    printf("%i Zeichen wurden geschrieben.\n", rc_sprintf);
  }

  // Die Zeichenkette im gemeinsamen Speichersegment ausgeben.
  if (printf("Inhalt von %s: %s\n", name, shmem_pointer) < 0) {
    printf("Der Lesezugriff ist fehlgeschlagen.\n");
    exit(1);
  }
  
  // Gemeinsames Speichersegment lösen
  if (munmap(shmem_pointer, MAXMEMSIZE) < 0) {
    printf("%s konnte nicht gelöst werden.\n", name);
    perror("munmap");
    exit(1);
  } else { 
    printf("%s wurde vom Prozess gelöst.\n", name);
  }
  
  // Gemeinsames Speichersegment schließen
  if (close(shmem_fd) < 0) {
    printf("%s konnte nicht geschlossen werden.\n", name);
    perror("close");
    exit(1);
  } else { 
    printf("%s wurde geschlossen.\n", name);
  }
      
  // Gemeinsames Speichersegment löschen
  if (shm_unlink(name) < 0) {
    printf("%s konnte nicht gelöscht werden.\n", name);
    perror("shm_unlink");
    exit(1);
  } else { 
    printf("%s wurde gelöscht.\n", name);
  }
    
  exit(0);
}
