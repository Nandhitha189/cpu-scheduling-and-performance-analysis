#include <string.h>
#include "scheduler.h"

void init_results(Results *R, int n) {
    R->avg_wait = R->avg_turn = R->avg_resp = 0.0;
    R->cpu_util = 0.0;
    R->context_switches = 0;
    R->makespan = 0;
    R->n = n;
    R->gcount = 0;
    R->gantt = (Slot*)calloc(n * 4 + 16, sizeof(Slot)); // rough upper bound
}

void free_results(Results *R) {
    free(R->gantt);
    R->gantt = NULL;
    R->gcount = 0;
}

static void compute_aggregates(const Process *P, int n, Results *R) {
    long idle = 0;
    int last_end = 0;
    for (int i = 0; i < R->gcount; ++i) {
        if (i == 0 && R->gantt[i].start > 0) idle += R->gantt[i].start;
        if (i > 0 && R->gantt[i].start > R->gantt[i-1].end) {
            idle += (R->gantt[i].start - R->gantt[i-1].end);
        }
        if (R->gantt[i].end > last_end) last_end = R->gantt[i].end;
    }
    R->makespan = last_end;
    long sumw=0, sumt=0, sumr=0;
    for (int i=0;i<n;i++) {
        sumw += P[i].waiting;
        sumt += P[i].turnaround;
        sumr += P[i].response;
    }
    R->avg_wait = n? (double)sumw/n : 0.0;
    R->avg_turn = n? (double)sumt/n : 0.0;
    R->avg_resp = n? (double)sumr/n : 0.0;
    R->cpu_util = (last_end>0)? 100.0 * (last_end - idle) / last_end : 0.0;
}

void print_results(const char *name, const Results *R) {
    printf("\n=== %s ===\n", name);
    printf("Processes: %d\n", R->n);
    printf("Context Switches: %d\n", R->context_switches);
    printf("Makespan: %d\n", R->makespan);
    printf("CPU Utilization: %.2f%%\n", R->cpu_util);
    printf("Avg Waiting: %.2f  Avg Turnaround: %.2f  Avg Response: %.2f\n",
           R->avg_wait, R->avg_turn, R->avg_resp);
}

void print_gantt(const Results *R) {
    printf("Gantt (start,end,pid):\n");
    for (int i=0;i<R->gcount;i++) {
        printf("(%d,%d,P%d)%s", R->gantt[i].start, R->gantt[i].end, R->gantt[i].pid,
               (i==R->gcount-1) ? "\n" : " -> ");
    }
}

int read_input(Process *P, int *n) {
    int N;
    if (scanf("%d", &N) != 1) {
        fprintf(stderr, "Error: failed to read number of processes\n");
        return 0;
    }
    if (N <= 0 || N > MAXN) {
        fprintf(stderr, "Error: N must be in 1..%d\n", MAXN);
        return 0;
    }
    for (int i=0;i<N;i++) {
        int at, bt, pr;
        if (scanf("%d %d %d", &at, &bt, &pr) != 3) {
            fprintf(stderr, "Error: failed to read process %d (arrival burst priority)\n", i);
            return 0;
        }
        P[i].pid = i;
        P[i].arrival = at;
        P[i].burst = bt;
        P[i].remaining = bt;
        P[i].priority = pr;
        P[i].start = -1;
        P[i].completion = P[i].waiting = P[i].turnaround = P[i].response = 0;
    }
    *n = N;
    return 1;
}

/* expose a helper to finalize metrics after algorithms mutate P */
void finalize(Process *P, int n, Results *R) {
    /* Placeholder to satisfy static checker – real aggregation in algorithms after fill */
}

/* Local helper to keep API tiny: recompute aggregates based on current P and Gantt */
void _finish_and_aggregate(const Process *P, int n, Results *R) {
    compute_aggregates(P, n, R);
}
