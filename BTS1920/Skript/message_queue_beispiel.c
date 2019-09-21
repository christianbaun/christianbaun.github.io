// title:        message_queue_beispiel.c
// description:  This c program is a very simple message queue example for Linux
// author:       Dr. Christian Baun
// url:          http://www.christianbaun.de
// license:      GPLv2
// date:         November 10th 2016
// version:      1.2
// gcc_version:  gcc (Debian 4.9.2-10) 4.9.2
// compile with: gcc message_queue_beispiel.c -o message_queue_beispiel 
// nodes:        This program creates a message queue, inserts a message into
//               the message queue, reads the message out of the message queue
//               and finally erases the message queue.
//               Check the status of existing message queues with command ipcs

#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/msg.h>
#include <string.h>                  // Diese Header-Datei ist nötig für strcpy()

typedef struct msgbuf {              // Template eines Puffers für msgsnd und msgrcv
    long mtype;                      // Nachrichtentyp
    char mtext[80];                  // Sendepuffer
} msg;

int main(int argc, char **argv) {
    int message_queue_key = 12345;
    int returncode_msgget;
    int returncode_msgctl;
    int returncode_msgrcv;
    msg sendbuffer, receivebuffer;   // Einen Empfangspuffer und einen Sendepuffer anlegen
    
    // Nachrichtenwarteschlange erzeugen oder auf eine bestehende zugreifen
    // IPC_CREAT = Warteschlange erzeugen, wenn sie noch nicht existiert
    // 0600 = Zugriffsrechte auf die neue Nachrichtenwarteschlange
    returncode_msgget = msgget(message_queue_key, IPC_CREAT | 0600);
    if(returncode_msgget < 0) {
        printf("Die Nachrichtenwarteschlange konnte nicht erstellt werden.\n");
        exit(1);
    } else {
        printf("Die Nachrichtenwarteschlange %i mit der ID %i ist nun verfügbar.\n", message_queue_key, returncode_msgget);
    }
    
    sendbuffer.mtype = 1;                            // Nachrichtentyp festlegen
    strncpy(sendbuffer.mtext, "Testnachricht", 13);  // Nachricht in den Sendepuffer schreiben

    // Eine Nachricht in die Nachrichtenwarteschlange schreiben
    if (msgsnd(returncode_msgget, &sendbuffer, sizeof(sendbuffer.mtext), IPC_NOWAIT) == -1) {
        printf("In die Nachrichtenwarteschlange konnte nicht geschrieben werden.\n");
        exit(1);
    } else {
       printf("Diese Nachricht wurde in die Nachrichtenwarteschlange geschrieben: %s\n", sendbuffer.mtext);
    }
    
    // Wenn mtype = 0, dann die erste Meldung aus der Nachrichtenwarteschlange lesen
    // Wenn mtype = positiv, dann die erste Meldung dieses Typs aus der Nachrichtenwarteschlange lesen
    // Wenn mtype = negativ, dann die erste aus der Nachrichtenwarteschlange lesen, deren Typ kleiner oder gleich dem absoluten Wert von mtype ist 

    // Die erste Nachricht vom Typ 1 in der Nachrichtenwarteschlangen empfangen
    receivebuffer.mtype = 1;        
    
    returncode_msgrcv = msgrcv(returncode_msgget, &receivebuffer, sizeof(receivebuffer.mtext), receivebuffer.mtype, MSG_NOERROR | IPC_NOWAIT);
    if (returncode_msgrcv < 0) {
        printf("Aus der Nachrichtenwarteschlange konnte nicht gelesen werden.\n");
        exit(1);
    } else {
        printf("Diese Nachricht wurde aus der Nachrichtenwarteschlange gelesen: %s\n", receivebuffer.mtext);
        printf("Die empfangene Nachricht ist %i Zeichen lang.\n", returncode_msgrcv);	
    }
    
    // Nachrichtenwarteschlange löschen
    returncode_msgctl = msgctl(returncode_msgget, IPC_RMID, 0);
    if (returncode_msgctl < 0) {
        printf("Die Nachrichtenwarteschlange %i mit der ID %i konnte nicht gelöscht werden.\n", message_queue_key, returncode_msgget);
        exit(1);
    } else { 
        printf("Die Nachrichtenwarteschlange %i mit der ID %i wurde gelöscht.\n", message_queue_key, returncode_msgget);
    }
    
    exit(0);
}