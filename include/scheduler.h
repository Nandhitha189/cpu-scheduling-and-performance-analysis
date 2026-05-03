#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>
#include <stdlib.h>

#define MAXN 128

typedef struct {
    int pid;
    int arrival;
    int burst;
    int remaining;
    int priority;
    int start;        // first time it got CPU
    int completion;   // finish time
    int waiting;
    int turnaround;
    int response;
} Process;

typedef struct {
    int start;
    int end;
    int pid;
} Slot;

typedef struct {
    double avg_wait, avg_turn, avg_resp, cpu_util;
    int context_switches;
    int makespan;
    int n;
    Slot *gantt;
    int gcount;
} Results;

void init_results(Results *R, int n);
void free_results(Results *R);
void print_results(const char *name, const Results *R);
void print_gantt(const Results *R);
int read_input(Process *P, int *n);

#endif // SCHEDULER_H
