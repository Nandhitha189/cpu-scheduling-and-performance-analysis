#include <string.h>
#include <limits.h>
#include "scheduler.h"
#include "algorithms.h"

static void copy_procs(const Process *in, int n, Process *out) {
    for (int i=0;i<n;i++) out[i] = in[i];
}

static void push_slot(Results *R, int s, int e, int pid) {
    if (R->gantt && e > s) {
        R->gantt[R->gcount].start = s;
        R->gantt[R->gcount].end = e;
        R->gantt[R->gcount].pid = pid;
        R->gcount++;
    }
}

static void aggregate(Process *P, int n, Results *R) {
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
        P[i].turnaround = P[i].completion - P[i].arrival;
        P[i].waiting = P[i].turnaround - P[i].burst;
        sumw += P[i].waiting;
        sumt += P[i].turnaround;
        sumr += P[i].response;
    }
    R->avg_wait = n? (double)sumw/n : 0.0;
    R->avg_turn = n? (double)sumt/n : 0.0;
    R->avg_resp = n? (double)sumr/n : 0.0;
    R->cpu_util = (last_end>0)? 100.0 * (last_end - idle) / last_end : 0.0;
}

void fcfs(const Process *in, int n, Results *R) {
    Process P[MAXN];
    copy_procs(in, n, P);
    init_results(R, n);

    // simple sort by arrival then pid (stable-ish)
    for (int i=0;i<n;i++) {
        for (int j=i+1;j<n;j++) {
            if (P[j].arrival < P[i].arrival || (P[j].arrival==P[i].arrival && P[j].pid < P[i].pid)) {
                Process t = P[i]; P[i] = P[j]; P[j] = t;
            }
        }
    }

    int t = 0, last_pid = -2;
    for (int i=0;i<n;i++) {
        if (t < P[i].arrival) { // idle
            t = P[i].arrival;
        }
        if (P[i].start == -1) {
            P[i].start = t;
            P[i].response = t - P[i].arrival;
        }
        push_slot(R, t, t + P[i].burst, P[i].pid);
        if (last_pid != -2 && last_pid != P[i].pid) R->context_switches++;
        last_pid = P[i].pid;
        t += P[i].burst;
        P[i].completion = t;
    }
    aggregate(P, n, R);
}

void implementSJF(const Process *in, int n, Results *R) {
    /* Non-preemptive Shortest Job First */
    Process P[MAXN];
    copy_procs(in, n, P);
    init_results(R, n);

    int completed = 0;
    int t = 0;
    int last_pid = -2;

    while (completed < n) {
        int idx = -1;
        int best_burst = INT_MAX;

        /* pick shortest job among arrived processes */
        for (int i = 0; i < n; ++i) {
            if (P[i].remaining > 0 && P[i].arrival <= t) {
                if (idx == -1 || P[i].burst < best_burst ||
                    (P[i].burst == best_burst && P[i].arrival < P[idx].arrival)) {
                    best_burst = P[i].burst;
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            /* no one has arrived yet – jump to next arrival */
            int next_t = INT_MAX;
            for (int i = 0; i < n; ++i) {
                if (P[i].remaining > 0 && P[i].arrival < next_t) {
                    next_t = P[i].arrival;
                }
            }
            if (next_t == INT_MAX) break; /* nothing left */
            t = next_t;
            continue;
        }

        if (P[idx].start == -1) {
            P[idx].start = t;
            P[idx].response = t - P[idx].arrival;
        }

        int run_time = P[idx].remaining;
        push_slot(R, t, t + run_time, P[idx].pid);
        if (last_pid != -2 && last_pid != P[idx].pid) {
            R->context_switches++;
        }
        last_pid = P[idx].pid;

        t += run_time;
        P[idx].remaining = 0;
        P[idx].completion = t;

        completed++;
    }

    aggregate(P, n, R);
}

void rr_fixed(const Process *in, int n, int quantum, Results *R) {
    if (quantum <= 0) quantum = 2;
    Process P[MAXN], Q[MAXN];
    copy_procs(in, n, P);
    init_results(R, n);
    int t=0, head=0, tail=0, completed=0, last_pid=-2;

    // push arrivals at time 0
    int arrived[MAXN]={0};
    for (int i=0;i<n;i++) arrived[i]=0;
    // Use index order for fairness
    while (completed < n) {
        // enqueue any arrivals at time t
        for (int i=0;i<n;i++) if (!arrived[i] && P[i].arrival <= t && P[i].remaining>0) {
            Q[tail++] = P[i]; arrived[i]=1;
        }
        if (head==tail) {
            // idle until next arrival
            int next_t = INT_MAX, idx=-1;
            for (int i=0;i<n;i++) if (P[i].remaining>0 && P[i].arrival < next_t) {
                next_t = P[i].arrival; idx=i;
            }
            if (idx==-1) break;
            t = next_t;
            if (!arrived[idx]) { Q[tail++] = P[idx]; arrived[idx]=1; }
            continue;
        }
        // pop
        Process cur = Q[head++];
        int pid = cur.pid;
        int slice = (cur.remaining < quantum) ? cur.remaining : quantum;
        // set start/response
        if (P[pid].start == -1) {
            P[pid].start = t;
            P[pid].response = t - P[pid].arrival;
        }
        push_slot(R, t, t+slice, pid);
        if (last_pid != -2 && last_pid != pid) R->context_switches++;
        last_pid = pid;
        t += slice;
        P[pid].remaining -= slice;
        // enqueue new arrivals that have come by time t
        for (int i=0;i<n;i++) if (!arrived[i] && P[i].arrival <= t && P[i].remaining>0) {
            Q[tail++] = P[i]; arrived[i]=1;
        }
        if (P[pid].remaining > 0) {
            Q[tail++] = P[pid];
        } else {
            P[pid].completion = t;
            completed++;
        }
    }
    aggregate(P, n, R);
}

void priority_preemptive(const Process *in, int n, int aging, Results *R) {
    Process P[MAXN];
    copy_procs(in, n, P);
    init_results(R, n);

    int t = 0, done = 0, last_pid = -2;
    int min_arr = INT_MAX;
    for (int i=0;i<n;i++) if (P[i].arrival < min_arr) min_arr = P[i].arrival;
    if (min_arr > 0) t = min_arr;

    while (done < n) {
        int best=-1, best_pr=INT_MAX;
        for (int i=0;i<n;i++) if (P[i].arrival <= t && P[i].remaining > 0) {
            int eff_pr = P[i].priority;
            if (aging > 0) {
                int wait = t - P[i].arrival - (P[i].burst - P[i].remaining);
                if (wait > 0) eff_pr -= wait / aging; // smaller is better
                if (eff_pr < -1000) eff_pr = -1000;
            }
            if (eff_pr < best_pr || (eff_pr==best_pr && P[i].arrival < P[best].arrival)) {
                best_pr = eff_pr;
                best = i;
            }
        }
        if (best == -1) {
            // jump to next arrival
            int next_t = INT_MAX;
            for (int i=0;i<n;i++) if (P[i].remaining>0 && P[i].arrival < next_t) next_t = P[i].arrival;
            if (next_t==INT_MAX) break;
            t = next_t;
            continue;
        }
        if (P[best].start == -1) {
            P[best].start = t;
            P[best].response = t - P[best].arrival;
        }
        // run 1 unit (preemptive at unit boundaries)
        push_slot(R, t, t+1, P[best].pid);
        if (last_pid != -2 && last_pid != P[best].pid) R->context_switches++;
        last_pid = P[best].pid;
        t += 1;
        P[best].remaining -= 1;
        if (P[best].remaining == 0) {
            P[best].completion = t;
            done++;
        }
    }
    aggregate(P, n, R);
}

void optimize_pick_best(const Process *in, int n, int rrq, int aging) {
    Results A, B, C;
    fcfs(in, n, &A);
    rr_fixed(in, n, rrq, &B);
    priority_preemptive(in, n, aging, &C);
    const char *best = "FCFS";
    double bestv = A.avg_wait;
    if (B.avg_wait < bestv) { bestv = B.avg_wait; best = "Round Robin"; }
    if (C.avg_wait < bestv) { bestv = C.avg_wait; best = "Priority Preemptive"; }
    print_results("FCFS", &A); print_gantt(&A); free_results(&A);
    print_results("Round Robin", &B); print_gantt(&B); free_results(&B);
    print_results("Priority Preemptive", &C); print_gantt(&C); free_results(&C);
    printf("\n>> Optimize suggests: %s (lowest average waiting time: %.2f)\n", best, bestv);
}

void analyzeAlgorithmEfficiency(const Process *in, int n, int rrq, int aging) {
    Results R_fcfs, R_rr, R_prio, R_sjf;

    fcfs(in, n, &R_fcfs);
    rr_fixed(in, n, rrq, &R_rr);
    priority_preemptive(in, n, aging, &R_prio);
    implementSJF(in, n, &R_sjf);

    printf("\n=== Comparative Efficiency Analysis ===\n");
    printf("%-22s %-10s %-10s %-10s %-10s %-8s %-9s\n",
           "Algorithm", "AvgWait", "AvgTurn", "AvgResp",
           "CPU(%)", "CS", "Makespan");
    printf("---------------------------------------------------------------------\n");
    printf("%-22s %-10.2f %-10.2f %-10.2f %-10.2f %-8d %-9d\n",
           "FCFS",
           R_fcfs.avg_wait, R_fcfs.avg_turn, R_fcfs.avg_resp,
           R_fcfs.cpu_util, R_fcfs.context_switches, R_fcfs.makespan);
    printf("%-22s %-10.2f %-10.2f %-10.2f %-10.2f %-8d %-9d\n",
           "Round Robin (RR)",
           R_rr.avg_wait, R_rr.avg_turn, R_rr.avg_resp,
           R_rr.cpu_util, R_rr.context_switches, R_rr.makespan);
    printf("%-22s %-10.2f %-10.2f %-10.2f %-10.2f %-8d %-9d\n",
           "Priority (Preemptive)",
           R_prio.avg_wait, R_prio.avg_turn, R_prio.avg_resp,
           R_prio.cpu_util, R_prio.context_switches, R_prio.makespan);
    printf("%-22s %-10.2f %-10.2f %-10.2f %-10.2f %-8d %-9d\n",
           "SJF (Non-preemptive)",
           R_sjf.avg_wait, R_sjf.avg_turn, R_sjf.avg_resp,
           R_sjf.cpu_util, R_sjf.context_switches, R_sjf.makespan);

    /* Pick the algorithm with the smallest average waiting time */
    const char *best = "FCFS";
    double bestv = R_fcfs.avg_wait;

    if (R_rr.avg_wait < bestv) { bestv = R_rr.avg_wait; best = "Round Robin (RR)"; }
    if (R_prio.avg_wait < bestv) { bestv = R_prio.avg_wait; best = "Priority (Preemptive)"; }
    if (R_sjf.avg_wait < bestv) { bestv = R_sjf.avg_wait; best = "SJF (Non-preemptive)"; }

    printf("\n>> Best by average waiting time: %s (%.2f)\n", best, bestv);

    free_results(&R_fcfs);
    free_results(&R_rr);
    free_results(&R_prio);
    free_results(&R_sjf);
}
