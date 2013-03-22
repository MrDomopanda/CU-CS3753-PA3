/*
 * File: mix-sched.c
 * Author: Stephen Bennett
 * Project: CSCI 3753 Programming Assignment 3
 * Create Date: 2013/03/21
 * Modify Date: 2013/03/21
 * Description:
 *  This file
 *
 */

/* Local Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <sys/types.h>  // Used for fork
#include <sys/wait.h>   // Used for fork
#include <unistd.h>     // Used for fork

/* Local Defines */
#define MAXFILENAMELENGTH 80
#define DEFAULT_INPUTFILENAME "rwinput"
#define DEFAULT_OUTPUTFILENAMEBASE "rwoutput"
#define DEFAULT_BLOCKSIZE 1024
#define DEFAULT_TRANSFERSIZE 1024*100
#define DEFAULT_CHILDREN 10

int main(int argc, char* argv[]){

    /* Parameters */
    int policy;
    int children;

    /* Scheduler/Fork Variables */
    struct sched_param param;
    int    i;
    pid_t  pid;

    /* Process program arguments to select run-time parameters */
    /* Set policy or default if not supplied */
    if (argc < 5) {
        policy = SCHED_OTHER;
    }
    else {
        if (!strcmp(argv[4], "SCHED_OTHER")) {
            policy = SCHED_OTHER;
        }
        else if (!strcmp(argv[4], "SCHED_FIFO")) {
            policy = SCHED_FIFO;
        }
        else if (!strcmp(argv[4], "SCHED_RR")) {
            policy = SCHED_RR;
        }
        else {
            fprintf(stderr, "Unhandeled scheduling policy [%s]\n", argv[4]);
            fprintf(stderr, "Available scheduling policies:\n");
            fprintf(stderr, "  SCHED_OTHER, SCHED_FIFO, SCHED_RR\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Set number of child processes to spawn or default if not supplied */
    if (argc < 6) {
        children = DEFAULT_CHILDREN;
    }
    else {
        children = atol(argv[5]);
        if (children == 0) {
            /* Set default iterations */
            children = DEFAULT_CHILDREN;
        }
        else if (children < 0) {
            fprintf(stderr, "Bad children value [%d]\n", children);
            fprintf(stderr, "  0:  Default children [%d]\n", DEFAULT_CHILDREN);
            fprintf(stderr, "  <0: Bad children value\n");
            fprintf(stderr, "  >0: Good children value\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Set process to max priority for given scheduler */
    param.sched_priority = sched_get_priority_max(policy);

    /* Set new scheduler policy */
    fprintf(stdout, "Current Scheduling Policy: %d\n", sched_getscheduler(0));
    fprintf(stdout, "Setting Scheduling Policy to: %d\n", policy);
    if (sched_setscheduler(0, policy, &param)) {
        perror("Error setting scheduler policy");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "New Scheduling Policy: %d\n", sched_getscheduler(0));

    /* Fork children child processes */
    for (i = 0; i < children; ++i) {
        if ((pid = fork()) == -1) exit(EXIT_FAILURE);   /* Fork Failed */

        if (pid == 0) { /* Child process */
            execv("mix", argv);
            exit(EXIT_SUCCESS);
        } else {        /* Parent process */
            printf("Forked %d pid = %d\n", i, pid);
        }
    }

    /* Wait for children child processes to finish */
    for (i = 0; i < children; ++i) {
        pid = wait(NULL);
        printf("Waited %d pid = %d\n", i, pid);
    }

    return 0;
}
