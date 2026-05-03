#ifndef ALGORITHMS_H
#define ALGORITHMS_H
#include "scheduler.h"

void fcfs(const Process *in, int n, Results *R);
void rr_fixed(const Process *in, int n, int quantum, Results *R);
void priority_preemptive(const Process *in, int n, int aging, Results *R);

void implementSJF(const Process *in, int n, Results *R);

/* Run all algorithms on the same input and print a compact comparison table */
void analyzeAlgorithmEfficiency(const Process *in, int n, int rrq, int aging);

/* Simple "optimize": run all three and report which had best avg waiting time */
void optimize_pick_best(const Process *in, int n, int rrq, int aging);

#endif // ALGORITHMS_H
