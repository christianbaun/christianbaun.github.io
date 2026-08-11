// title:        Listing_9_3_message_queue_systemv.c
// description:  This c program is an example about inter-process communication
//               via a message queue (System V) for Linux
// author:       Dr. Christian Baun
// url:          http://www.christianbaun.de
// license:      GPLv2
// date:         October 9th 2021
// version:      1.4
// gcc_version:  gcc 10.2.1 (Debian 10.2.1-6)
// compile with: gcc Listing_9_3_message_queue_systemv.c -o Listing_9_3_message_queue_systemv
// nodes:        This program creates a System V message queue, inserts a 
//               message into queue, reads the message out of the queue and
//               finally erases the message queue.
//               Check the status of existing System V message queues with the 
//               command ipcs -q

#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/msg.h>
#include <string.h>                  // für strcpy

typedef struct msgbuf {              // Template eines Puffers für msgsnd und msgrcv
  long mtype;                        // Nachrichtentyp
  char mtext[80];                    // Sendepuffer
} msg;

int main(int argc, char **argv) {
  int mq_key = 12345;              // Message Queue Key
  int rc_msgget;                   // Rückgabewert (return code) von msgget
  int rc_msgctl;                   // Rückgabewert (return code) von msgctl
  int rc_msgrcv;                   // Rückgabewert (return code) von msgrcv
  msg sendbuffer, receivebuffer;   // Einen Empfangspuffer und einen Sendepuffer anlegen
    
  // Nachrichtenwarteschlange erzeugen oder auf eine bestehende zugreifen
  // IPC_CREAT = Warteschlange erzeugen, wenn sie noch nicht existiert
  // 0600 = Zugriffsrechte auf die neue Nachrichtenwarteschlange
  rc_msgget = msgget(mq_key, IPC_CREAT | 0600);
  if(rc_msgget < 0) {
    printf("Die Warteschlange konnte nicht erstellt werden.\n");
    perror("msgget");
    exit(1);
  } else {
    printf("Nachrichtenwarteschlange %i mit ID %i ist bereit.\n", 
            mq_key, rc_msgget);
  }
    
  sendbuffer.mtype = 1;                            // Nachrichtentyp festlegen
  strncpy(sendbuffer.mtext, "Testnachricht", 13);  // Nachricht in den Sendepuffer schreiben

  // Eine Nachricht in die Nachrichtenwarteschlange schreiben
  // (senden)
  if (msgsnd(rc_msgget, 
              &sendbuffer, 
              sizeof(sendbuffer.mtext), 
              IPC_NOWAIT) == -1) {
    printf("Das Senden der Nachricht ist fehlgeschlagen.\n");
    perror("msgsnd");
    exit(1);
  } else {
    printf("Geschriebene Nachricht: %s\n", sendbuffer.mtext);
  }
    
  // Wenn mtype = 0, dann die erste Meldung aus der Nachrichtenwarteschlange lesen
  // Wenn mtype = positiv, dann die erste Meldung dieses Typs aus der Nachrichtenwarteschlange lesen
  // Wenn mtype = negativ, dann die erste aus der Nachrichtenwarteschlange lesen, deren Typ kleiner oder gleich dem absoluten Wert von mtype ist 

  // Die erste Nachricht vom Typ 1 aus der Nachrichtenwarteschlangen
  // lesen (empfangen)
  receivebuffer.mtype = 1;        
  
  rc_msgrcv = msgrcv(rc_msgget, 
                      &receivebuffer, 
                      sizeof(receivebuffer.mtext), 
                      receivebuffer.mtype, 
                      MSG_NOERROR | IPC_NOWAIT);
  if (rc_msgrcv < 0) {
    printf("Lesen der Nachricht fehlgeschlagen.\n");
    perror("msgrcv");
    exit(1);
  } else {
    printf("Empfangene Nachricht: %s\n", receivebuffer.mtext);
    printf("Länge der Nachricht: %i Zeichen.\n", rc_msgrcv);
  }
    
  // Nachrichtenwarteschlange löschen
  rc_msgctl = msgctl(rc_msgget, IPC_RMID, 0);
  if (rc_msgctl < 0) {
    printf("Die Warteschlange konnte nicht gelöscht werden.\n");
    perror("msgctl");
    exit(1);
  } else { 
    printf("%i mit ID %i wurde gelöscht.\n", mq_key, rc_msgget);
  }
  
  exit(0);
}
