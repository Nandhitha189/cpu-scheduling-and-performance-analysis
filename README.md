# CPU Scheduling & Optimization Simulator

A system-level CPU scheduling simulator implemented in C that models and compares multiple scheduling algorithms. The project performs detailed performance analysis using metrics such as waiting time, turnaround time, and throughput to identify the most efficient scheduling strategy.

The simulator supports FCFS, Round Robin, Priority Scheduling with aging, SJF, and optimization/analysis modes. It records execution intervals for a Gantt chart and computes metrics such as waiting time, turnaround time, and response time.

## Key Features
- Implementation of FCFS, Round Robin, Priority Scheduling (with aging)
- Performance analysis using waiting time, turnaround time, and throughput
- Gantt chart simulation for execution visualization
- Optimization mode to compare scheduling strategies
- Modular multi-file architecture with no global variables

## Key Highlights
- Demonstrates core operating system concepts through practical implementation
- Compares algorithm efficiency under different workloads
- Designed with clean modular architecture and no global variables
- Bridges theoretical scheduling concepts with real-world simulation

## Tech Stack
- C programming
- Data Structures and Algorithms
- Operating Systems concepts
- Makefile-based build system

## Project Structure
```text
include/
  scheduler.h
  algorithms.h
src/
  main.c
  algorithms.c
  io.c
Makefile
README.md
```

## How to Build
```bash
make
```

## How to Run
```bash
./a.out
```

## Input Format
```text
N
arrival_time burst_time priority
arrival_time burst_time priority
...
```

## Example Input
```text
4
0 5 3
1 3 2
2 8 1
3 6 4
```

For Round Robin, enter a time quantum when prompted. For Priority Scheduling, enter an aging factor. Use `0` to disable aging.

## My Contributions
- Implemented multiple CPU scheduling algorithms in C.
- Designed modular scheduling logic using separate source and header files.
- Added performance metric calculation for waiting, turnaround, and response time.
- Added Gantt chart recording to visualize process execution order.
- Implemented optimization/analysis logic to compare scheduling strategies.

## Learning Outcome
This project helped me understand CPU scheduling, process management, algorithm comparison, and clean modular C programming for operating-system simulations.
