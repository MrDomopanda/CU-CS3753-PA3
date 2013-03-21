/*
 * File: pi-sched.c
 * Author: Andy Sayler
 * Project: CSCI 3753 Programming Assignment 3
 * Create Date: 2012/03/07
 * Modify Date: 2012/03/09
 * Description:
 *  This file contains a simple program for statistically
 *      calculating pi using a specific scheduling policy.
 */

/* Local Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sched.h>
#include <sys/types.h>  // Used for fork
#include <sys/wait.h>   // Used for fork
#include <unistd.h>     // Used for fork

#define DEFAULT_ITERATIONS 1000000
#define DEFAULT_CHILDREN   10
#define RADIUS (RAND_MAX / 2)

inline double dist(double x0, double y0, double x1, double y1){
    return sqrt( pow((x1-x0), 2) + pow((y1-y0), 2) );
}

inline double zeroDist(double x, double y){
    return dist(0, 0, x, y);
}

int main(int argc, char* argv[]){

    long   i;
    long   iterations;
    struct sched_param param;
    int    policy;
    int    children;
    double x, y;
    double inCircle = 0.0;
    double inSquare = 0.0;
    double pCircle  = 0.0;
    double piCalc   = 0.0;
    pid_t pid;

    /* Process program arguments to select iterations and policy */
    /* Set default iterations if not supplied */
    if (argc < 2) {
        iterations = DEFAULT_ITERATIONS;
    }

    /* Set default policy if not supplied */
    if (argc < 3) {
        policy = SCHED_OTHER;
    }

    /* Set default number of child processes to spawn */
    if (argc < 4) {
        children = DEFAULT_CHILDREN;
    }

    /* Set iterations if supplied */
    if (argc > 1) {
        iterations = atol(argv[1]);
        if (iterations == 0) {
            /* Set default iterations */
            iterations = DEFAULT_ITERATIONS;
        }
        else if (iterations < 0) {
            fprintf(stderr, "Bad iterations value [%li]\n", iterations);
            fprintf(stderr, "  0:  Default iterations [%d]\n", DEFAULT_ITERATIONS);
            fprintf(stderr, "  <0: Bad iterations value\n");
            fprintf(stderr, "  >0: Good iterations value\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Set policy if supplied */
    if (argc > 2) {
        if (!strcmp(argv[2], "SCHED_OTHER")) {
            policy = SCHED_OTHER;
        }
        else if (!strcmp(argv[2], "SCHED_FIFO")) {
            policy = SCHED_FIFO;
        }
        else if (!strcmp(argv[2], "SCHED_RR")) {
            policy = SCHED_RR;
        }
        else {
            fprintf(stderr, "Unhandeled scheduling policy [%s]\n", argv[2]);
            fprintf(stderr, "Available scheduling policies:\n");
            fprintf(stderr, "  SCHED_OTHER, SCHED_FIFO, SCHED_RR\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Set children if supplied */
    if (argc > 3) {
        children = atol(argv[3]);
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

    /* Fork children */
    i = 0;
    do {
        ++i;
        if ((pid = fork()) == -1) exit(EXIT_FAILURE);   /* Fork Failed */
        if (pid > 0) printf("Forked %li pid = %d\n", i, pid);
    } while ((pid > 0) && (i < children));

    /* Calculate pi using statistical method across all iterations */
    for (i = 0; i < iterations; ++i) {
        x = (random() % (RADIUS * 2)) - RADIUS;
        y = (random() % (RADIUS * 2)) - RADIUS;
        if (zeroDist(x, y) < RADIUS) {
            inCircle++;
        }
        inSquare++;
    }

    /* Finish calculation */
    pCircle = inCircle/inSquare;
    piCalc = pCircle * 4.0;

    /* Print result */
    fprintf(stdout, "pi = %f\n", piCalc);

    /* Wait for children to finish */
    if (pid > 0) {
        for (i = 0; i < children; ++i) {
            pid = wait(NULL);
            printf("Waited %li pid = %d\n", i+1, pid);
        }
    }

    return 0;
}
