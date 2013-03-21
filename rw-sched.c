/*
 * File: rw-sched.c
 * Author: Stephen Bennett
 * Project: CSCI 3753 Programming Assignment 3
 * Create Date: 2013/03/21
 * Modify Date: 2013/03/21
 * Description:
 *  This file contains a simple program for statistically
 *      calculating pi using a specific scheduling policy.
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

    ssize_t transfersize = 0;
    ssize_t blocksize    = 0;
    char    inputFilename[MAXFILENAMELENGTH];
    char    outputFilenameBase[MAXFILENAMELENGTH];
    int     policy;
    int     children;

    struct sched_param param;
    int    i;
    pid_t  pid;

    /* Process program arguments to select run-time parameters */
    /* Set supplied transfer size or default if not supplied */
    if (argc < 2) {
        transfersize = DEFAULT_TRANSFERSIZE;
    }
    else {
        transfersize = atol(argv[1]);
        if (transfersize == 0) {
            transfersize = DEFAULT_TRANSFERSIZE;
        }
        else if (transfersize < 0) {
            fprintf(stderr, "Bad transfersize value\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Set supplied block size or default if not supplied */
    if (argc < 3) {
        blocksize = DEFAULT_BLOCKSIZE;
    }
    else {
        blocksize = atol(argv[2]);
        if (blocksize == 0) {
            blocksize = DEFAULT_BLOCKSIZE;
        }
        else if (blocksize < 0) {
            fprintf(stderr, "Bad blocksize value\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Set supplied input filename or default if not supplied */
    if (argc < 4) {
        if (strnlen(DEFAULT_INPUTFILENAME, MAXFILENAMELENGTH) >= MAXFILENAMELENGTH) {
            fprintf(stderr, "Default input filename too long\n");
            exit(EXIT_FAILURE);
        }
        strncpy(inputFilename, DEFAULT_INPUTFILENAME, MAXFILENAMELENGTH);
    }
    else {
        if (strnlen(argv[3], MAXFILENAMELENGTH) >= MAXFILENAMELENGTH) {
            fprintf(stderr, "Input filename too long\n");
            exit(EXIT_FAILURE);
        }
        strncpy(inputFilename, argv[3], MAXFILENAMELENGTH);
    }

    /* Set supplied output filename base or default if not supplied */
    if (argc < 5) {
        if (strnlen(DEFAULT_OUTPUTFILENAMEBASE, MAXFILENAMELENGTH) >= MAXFILENAMELENGTH) {
            fprintf(stderr, "Default output filename base too long\n");
            exit(EXIT_FAILURE);
        }
        strncpy(outputFilenameBase, DEFAULT_OUTPUTFILENAMEBASE, MAXFILENAMELENGTH);
    }
    else {
        if (!strcmp(argv[4], "default")) {
            strncpy(outputFilenameBase, DEFAULT_OUTPUTFILENAMEBASE, MAXFILENAMELENGTH);
        }
        else {
            if (strnlen(argv[4], MAXFILENAMELENGTH) >= MAXFILENAMELENGTH) {
                fprintf(stderr, "Output filename base is too long\n");
                exit(EXIT_FAILURE);
            }
            strncpy(outputFilenameBase, argv[4], MAXFILENAMELENGTH);
        }
    }

    /* Set policy or default if not supplied */
    if (argc < 6) {
        policy = SCHED_OTHER;
    }
    else {
        if (!strcmp(argv[5], "SCHED_OTHER")) {
            policy = SCHED_OTHER;
        }
        else if (!strcmp(argv[5], "SCHED_FIFO")) {
            policy = SCHED_FIFO;
        }
        else if (!strcmp(argv[5], "SCHED_RR")) {
            policy = SCHED_RR;
        }
        else {
            fprintf(stderr, "Unhandeled scheduling policy [%s]\n", argv[5]);
            fprintf(stderr, "Available scheduling policies:\n");
            fprintf(stderr, "  SCHED_OTHER, SCHED_FIFO, SCHED_RR\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Set number of child processes to spawn or default if not supplied */
    if (argc < 7) {
        children = DEFAULT_CHILDREN;
    }
    else {
        children = atol(argv[6]);
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
            // execl(exe, argv[0], argv[1], argv[2], ..., NULL)
            execl("rw", "rw", argv[1], argv[2], argv[3], argv[4], NULL);
            exit(EXIT_SUCCESS);
        } else {        /* Parent process */
            printf("Forked %d pid = %d\n", i, pid);
        }
    }

    /* Wait for children child processes to finish */
    for (i = 0; i < children; ++i) {
        pid = wait(NULL);
        printf("Waited %d pid = %d\n", i+1, pid);
    }

    return 0;
}
