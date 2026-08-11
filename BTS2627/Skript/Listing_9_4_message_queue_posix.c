// title:        Listing_9_4_message_queue_posix.c
// description:  This c program is an example about inter-process communication
//               via a message queue (POSIX) for Linux
// author:       Dr. Christian Baun
// url:          http://www.christianbaun.de
// license:      GPLv2
// date:         October 9th 2021
// version:      1.0
// gcc_version:  gcc 10.2.1 (Debian 10.2.1-6)
// compile with: gcc Listing_9_4_message_queue_posix.c -o Listing_9_4_message_queue_posix -lrt
// nodes:        This program creates a POSIX message queue, inserts a message 
//               into it, reads the message out of it and finally erases it.
//               POSIX message queues are created in Linux in the folder 
//               /dev/mqueue

#include <stdio.h>      // for printf
#include <stdlib.h>     // for exit
#include <string.h>     // for strcpy
#include <mqueue.h>     // for mq_open

int main(int argc, char **argv) {
  int rc_mq_receive;        // Rückgabewert (return code) von mq_receive
  const char mq_name[] = "/myqueue";
  char nachricht[] = "Testnachricht";
  
  mqd_t mymq_descriptor;    // Message queue descriptor
  int msg_prio = 0;         // Nachrichtenpriorität festlegen
  char recv_buffer[80];     // Empfangspuffer

  struct mq_attr attr;
  attr.mq_flags = 0;        // Flags. Ignored for mq_open 
  attr.mq_maxmsg = 10;      // Maximum number of messages on queue 
  attr.mq_msgsize = 80;     // Maximum message size (Bytes) 
  attr.mq_curmsgs = 0;      // Number of messages in the queue. Ignored for mq_open

  // Speicherbereich des Empfangspuffers mit Nullen füllen
  memset(&recv_buffer, 0, sizeof(recv_buffer)); 
  
  // Nachrichtenwarteschlange erzeugen oder auf eine bestehende zugreifen
  // O_CREAT = Warteschlange erzeugen, wenn sie noch nicht existiert
  // O_RDWR = Nachrichtenwarteschlange lesend und schreibend öffnen
  // O_NONBLOCK = Die Nachrichtenwarteschlange nichtblockierend öffnen
  // 0600 = Zugriffsrechte auf die neue Nachrichtenwarteschlange
  mymq_descriptor = mq_open(mq_name, 
                            O_RDWR | O_CREAT | O_NONBLOCK, 
                            0600, 
                            &attr);
  if (mymq_descriptor < 0) {
    printf("Die Warteschlange konnte nicht erstellt werden.\n");
    perror("mq_open");
    exit(1);
  } else {
    printf("Nachrichtenwarteschlange %s ist verfügbar.\n", mq_name);
  }
      
  // Eine Nachricht in die Nachrichtenwarteschlange schreiben
  if (mq_send(mymq_descriptor, 
              nachricht, 
              strlen(nachricht), 
              msg_prio) == -1) {
    printf("Schreiben der Nachricht fehlgeschlagen");
    perror("mq_send");
    exit(1);
  } else {
    printf("Diese Nachricht wurde geschrieben: %s\n", nachricht);
  }
  
  // Eigenschaften der Nachrichtenwarteschlange abfragen
  if (mq_getattr(mymq_descriptor, &attr) == -1) {
    printf("Die Eigenschaften konnten nicht abgefragt werden.\n");
    perror("mq_getattr");
  }  
    
  // Anzahl der Nachrichten in der der Nachrichtenwarteschlange abfragen
  if (attr.mq_curmsgs == 0) {
    printf("%s enthält keine Nachrichten.\n", mq_name);
  } else if (attr.mq_curmsgs == 1) {
    printf("%s enthält %d Nachricht.\n", mq_name, attr.mq_curmsgs);
  } else {
    printf("Nachrichten in %s: %d\n", mq_name, attr.mq_curmsgs);
  }
      
  // Die älteste Nachricht mit der höchsten Priorität empfangen
  rc_mq_receive = mq_receive(mymq_descriptor, 
                             recv_buffer, 
                             sizeof(recv_buffer), 
                             &msg_prio);
  if (rc_mq_receive < 0) {
    printf("Lesen der Nachricht fehlgeschlagen.\n");
    perror("mq_receive");
  } else {
    printf("Empfangene Nachricht: %s\n", recv_buffer);
    printf("Die Nachricht ist %i Zeichen lang.\n", rc_mq_receive);	
  }
  
  // Nachrichtenwarteschlange schließen
  if (mq_close(mymq_descriptor) < 0) {
    printf("%s konnte nicht geschlossen werden.\n", mq_name);
    perror("mq_close");
    exit(1);
  } else { 
    printf("%s wurde geschlossen.\n", mq_name);
  }
  
  // Nachrichtenwarteschlange löschen
  if (mq_unlink(mq_name) < 0) {
    printf("%s konnte nicht gelöscht werden.\n", mq_name);
    perror("mq_unlink");
    exit(1);
  } else { 
    printf("%s wurde gelöscht.\n", mq_name);
  }

  exit(0);
}
