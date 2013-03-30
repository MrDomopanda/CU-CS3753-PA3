/*
 * File:        mix.c
 * Author:      Stephen Bennett
 * Project:     CSCI 3753 Programming Assignment 3
 * Create Date: 2013/03/21
 * Modify Date: 2013/03/21
 * Description:
 *  This file contains a small program for statistically calculating pi
 *      as well as writing the intermediate results of this calculation
 *      to a file.
 */

/* Local Includes */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>     // getpid()
#include <errno.h>
#include <fcntl.h>      // open()
#include <string.h>
#include <math.h>

/* Local Defines */
#define MAXFILENAMELENGTH 80
#define DEFAULT_OUTPUTFILENAMEBASE "mixoutput"
#define DEFAULT_ITERATIONS 10000
#define DEFAULT_INTERMEDIATERESULTS 100
#define RADIUS (RAND_MAX / 2)

/* Local Functions */
inline double dist(double x0, double y0, double x1, double y1){
    return sqrt( pow((x1 - x0), 2) + pow((y1 - y0), 2) );
}

inline double zeroDist(double x, double y){
    return dist(0, 0, x, y);
}

int main(int argc, char* argv[]) {

    /* Parameters */
    long intermediateResults;
    long iterations;
    char outputFilename[MAXFILENAMELENGTH];
    char outputFilenameBase[MAXFILENAMELENGTH];

    /* I/O Bound Variables */
    int  rv;
    int  outputFD;
    char writeBuffer[78];

    /* CPU Bound Variables */
    long   i, j;
    double x, y;
    double inCircle = 0.0;
    double inSquare = 0.0;
    double pCircle  = 0.0;
    double piCalc   = 0.0;

    /* Process program arguments to run-time parameters */
    /* Set intermediate results or default if not supplied */
    if (argc < 2) {
        intermediateResults = DEFAULT_INTERMEDIATERESULTS;
    }
    else {
        intermediateResults = atol(argv[1]);
        if (intermediateResults== 0) {
            intermediateResults = DEFAULT_INTERMEDIATERESULTS;
        }
        else if (intermediateResults < 0) {
            fprintf(stderr, "Bad intermediate results value [%li]\n", intermediateResults);
            fprintf(stderr, "  0:  Default intermediate results [%d]\n", DEFAULT_INTERMEDIATERESULTS);
            fprintf(stderr, "  <0: Bad intermediate results value\n");
            fprintf(stderr, "  >0: Good intermediate results value\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Set iterations or default if not supplied */
    if (argc < 3) {
        iterations = DEFAULT_ITERATIONS;
    }
    else {
        iterations = atol(argv[2]);
        if (iterations == 0) {
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

    /* Set supplied output filename base or default if not supplied */
    if (argc < 4) {
        if (strnlen(DEFAULT_OUTPUTFILENAMEBASE, MAXFILENAMELENGTH) >= MAXFILENAMELENGTH) {
            fprintf(stderr, "Default output filename base too long\n");
            exit(EXIT_FAILURE);
        }
        strncpy(outputFilenameBase, DEFAULT_OUTPUTFILENAMEBASE, MAXFILENAMELENGTH);
    }
    else {
        if (!strcmp(argv[3], "default")) {
            strncpy(outputFilenameBase, DEFAULT_OUTPUTFILENAMEBASE, MAXFILENAMELENGTH);
        }
        else {
            if (strnlen(argv[3], MAXFILENAMELENGTH) >= MAXFILENAMELENGTH) {
                fprintf(stderr, "Output filename base is too long\n");
                exit(EXIT_FAILURE);
            }
            strncpy(outputFilenameBase, argv[3], MAXFILENAMELENGTH);
        }
    }

    /* Open Output File Descriptor in Write Only mode with standard permissions */
    rv = snprintf(outputFilename, MAXFILENAMELENGTH, "%s-%d",
            outputFilenameBase, getpid());
    if (rv > MAXFILENAMELENGTH) {
        fprintf(stderr, "Output filename length exceeds limit of %d characters.\n",
                MAXFILENAMELENGTH);
        exit(EXIT_FAILURE);
    }
    else if (rv < 0) {
        perror("Failed to generate output filename");
        exit(EXIT_FAILURE);
    }

    if ((outputFD =
                open(outputFilename,
                    O_WRONLY | O_CREAT | O_TRUNC | O_SYNC,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)) < 0) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < intermediateResults; ++j) {
        /* Calculate pi using statistical method across all iterations*/
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
        piCalc  = pCircle * 4.0;

        snprintf(writeBuffer, 78, "In Circle Count = %-58f\n", inCircle);
        write(outputFD, writeBuffer, 77);
        snprintf(writeBuffer, 78, "In Square Count = %-58f\n", inSquare);
        write(outputFD, writeBuffer, 77);
        snprintf(writeBuffer, 78, "Calculated pi  = %-58f\n\n", piCalc);
        write(outputFD, writeBuffer, 77);
    }

    /* Close Output File Descriptor */
    if (close(outputFD)) {
        perror("Failed to close output file");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
