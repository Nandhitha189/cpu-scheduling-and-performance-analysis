#include <string.h>
#include "scheduler.h"
#include "algorithms.h"

static void example_banner(void){
    printf("Input format:\n");
    printf("N\n");
    printf("arrival burst priority  (one line per process)\n");
    printf("Example:\n");
    printf("4\n");
    printf("0 5 3\n1 3 2\n2 8 1\n3 6 4\n");
}

int main(void) {
    int choice;
    printf("=== Jackfruit Problem - CPU Scheduler (Student Edition) ===\n");
    printf("1) FCFS\n2) Round Robin (fixed quantum)\n3) Priority (preemptive)\n4) Optimize (pick best)\n5) SJF (non-preemptive)\n6) Analyze algorithms\n7) Help (input format)\n0) Exit\n");
    printf("Enter choice: ");
    if (scanf("%d", &choice) != 1) { fprintf(stderr, "Invalid input\n"); return 1; }

    if (choice == 0) return 0;
    if (choice == 7) { example_banner(); return 0; }

    Process P[MAXN]; int n=0;
    printf("Enter processes (see Help for format):\n");
    if (!read_input(P, &n)) return 1;

    if (choice == 1) {
        Results R; fcfs(P, n, &R);
        print_results("FCFS", &R); print_gantt(&R); free_results(&R);
    } else if (choice == 2) {
        int q; printf("Enter quantum: ");
        if (scanf("%d", &q) != 1 || q <= 0) { fprintf(stderr, "Bad quantum\n"); return 1; }
        Results R; rr_fixed(P, n, q, &R);
        print_results("Round Robin", &R); print_gantt(&R); free_results(&R);

    } else if (choice == 3) {
        int aging; printf("Enter aging factor (e.g., 5; 0 to disable): ");
        if (scanf("%d", &aging) != 1) { fprintf(stderr, "Bad aging\n"); return 1; }
        Results R; priority_preemptive(P, n, aging, &R);
        print_results("Priority Preemptive", &R); print_gantt(&R); free_results(&R);
    } else if (choice == 4) {
        int q, aging;
        printf("Enter quantum for RR and aging for Priority (e.g., 2 5): ");
        if (scanf("%d %d", &q, &aging) != 2) { fprintf(stderr, "Bad params\n"); return 1; }
        optimize_pick_best(P, n, q, aging);
    } else if (choice == 5) {
        Results R; implementSJF(P, n, &R);
        print_results("SJF (Non-preemptive)", &R); print_gantt(&R); free_results(&R);
    } else if (choice == 6) {
        int q, aging;
        printf("Enter quantum for RR and aging for Priority (e.g., 2 5): ");
        if (scanf("%d %d", &q, &aging) != 2) { fprintf(stderr, "Bad params\n"); return 1; }
        analyzeAlgorithmEfficiency(P, n, q, aging);
    } else {
        fprintf(stderr, "Unknown choice\n");
        return 1;
    }
    return 0;
}
