/* C89 that does one of two things:
 * - server: waits for the Orchestra, and plays whatever the Orchestra wants to
 *    play - even if it is totally discordant.
 * - client: asks the server politely to play some music, one note at a time.
 */

#include <errno.h>
#include <limits.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "music.h"

#define MAX_CLIENTS 4096
#define SERVER_PORT 1990
#define SOCK_BACKLOG 16
#define QUEUE_SIZE 1000

struct ThreadComms
{
    int* exit;
    /* main <--> musician */
    int socket;
    /* musician <--> instrument */
    pthread_mutex_t queueEx;
    struct Note queue[QUEUE_SIZE];  /* For storing musical notes */
};

void inc_queue_locator(int* queueLoc)
{
    *queueLoc = *queueLoc == QUEUE_SIZE - 1 ? 0 : *queueLoc + 1;
}

static void* instrument_thread_exec(void* arg)
{
    struct ThreadComms* comms = (struct ThreadComms*)(arg);
    int queueLoc = 0;
    struct Note note;
    struct AudioState* as = setup();

    while (comms->exit)
    {
        pthread_mutex_lock(&(comms->queueEx));
        note = comms->queue[queueLoc];
        /* Pass until non-zero pitch (hopefully intentionally) */
        if (note.pitch != 0)
        {
            comms->queue[queueLoc].pitch = 0;
            comms->queue[queueLoc].length = 0;
            comms->queue[queueLoc].breath = 0;
            /* Exit on invalid breath (hopefully intentionally) */
            if (note.breath == UINT_MAX) *(comms->exit) = 1;
            else
            {
                /*
                printf("%d: %f, %u, %u\n",
                       queueLoc, note.pitch, note.length, note.breath);
                */
                play_note(as, note);
            }
            inc_queue_locator(&queueLoc);
        }
        pthread_mutex_unlock(&(comms->queueEx));
    }
    teardown(as);
    return 0;
}

static void* musician_thread_exec(void* arg)
{
    struct ThreadComms* comms = (struct ThreadComms*)(arg);
    struct Note note;
    int queueLoc = 0;
    void* unused;

    pthread_t instrumentThread;
    if (pthread_create(
            &instrumentThread, NULL, &instrument_thread_exec, comms))
    {
        printf("A: %s\n", strerror(errno));
        return NULL;
    }

    while (comms->exit)
    {
        read(comms->socket, (void*)&note, sizeof(struct Note));
        /* Pass on zero pitch (hopefully intentionally) */
        if (note.pitch != 0)
        {
            pthread_mutex_lock(&(comms->queueEx));
            /*
            printf("M staging note %d with pitch %f\n", queueLoc, note.pitch);
            */
            comms->queue[queueLoc] = note;
            pthread_mutex_unlock(&(comms->queueEx));
            inc_queue_locator(&queueLoc);
            note.pitch = 0;
        }
    }

    pthread_join(instrumentThread, &unused);
    return 0;
}

int server(void)
{
    struct sockaddr_in sin;
    socklen_t sizeSin;  /* I know... */
    int masterSocket;
    int clientCount = 0;
    pthread_t threads[MAX_CLIENTS];
    int slaveSocket;
    struct ThreadComms threadComms[MAX_CLIENTS];
    int exit = 0;
    void* unused;

    sin.sin_family=AF_INET;
    sin.sin_addr.s_addr=INADDR_ANY;
    sin.sin_port=htons(SERVER_PORT);
    sizeSin = sizeof(sin);

    masterSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (masterSocket < 0)
    {
        printf("B: %s\n", strerror(errno));
        return 1;
    }

    if(bind(masterSocket, (struct sockaddr*)&sin, sizeSin) < 0)
    {
        printf("C: %s\n", strerror(errno));
        close(masterSocket);
        return 1;
    }

    if(listen(masterSocket, SOCK_BACKLOG) < 0)
    {
        printf("D: %s\n", strerror(errno));
        close(masterSocket);
        return 1;
    }

    /* Spawn one thread per client - accept is blocking. */
    while (clientCount < MAX_CLIENTS || threadComms->exit)
    {
        slaveSocket = accept(
            masterSocket, (struct sockaddr*)&sin, &sizeSin);
        if (slaveSocket < 0)
        {
            printf("E: %s\n", strerror(errno));
            close(masterSocket);
            return 1;
        }

        threadComms[clientCount].exit = &exit;
        threadComms[clientCount].socket = slaveSocket;
        if (pthread_create(
                &threads[clientCount], NULL, &musician_thread_exec,
                &threadComms[clientCount]))
        {
            printf("F: %s\n", strerror(errno));
            close(masterSocket);
            return 1;
        }
        clientCount++;
    }

    /* Join with each thread */
    for (; clientCount > 0; clientCount--)
        pthread_join(threads[clientCount - 1], &unused);

    close(masterSocket);
    return 0;
}

int client(int who)
{
    int ssocket;
    struct sockaddr_in sin;
    struct Note note;

    bzero((char*)&sin, sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_addr.s_addr=INADDR_ANY;
    sin.sin_port=htons(SERVER_PORT);

    ssocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ssocket < 0)
    {
        printf("G: %s\n", strerror(errno));
        return 1;
    }

    if (connect(ssocket, (struct sockaddr*)&sin, sizeof(sin)) < 0)
    {
        printf("H: %s\n", strerror(errno));
        close(ssocket);
        return 1;
    }

    /* Let it rip */
    if (who == 4096)
	{
#include "sheet_4096"
	}
    else if (who == 1)
    {
#include "sheet_1"
    }
    else if (who == 2)
    {
#include "sheet_2"
    }
    return 0;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Supply exactly one natural number (or 0) argument.\n");
        return 1;
    }
    if (atoi(argv[1]) == 0)    return server();
    else return client(atoi(argv[1]));
}
