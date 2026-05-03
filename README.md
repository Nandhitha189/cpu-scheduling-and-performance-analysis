# CPU Scheduling & Optimization Simulator

## Overview
This project is a CPU scheduling simulator written in C. It models how an operating system schedules processes and compares different scheduling strategies using standard performance metrics.

The simulator supports FCFS, Round Robin, Priority Scheduling with aging, SJF, and optimization/analysis modes. It records execution intervals for a Gantt chart and computes metrics such as waiting time, turnaround time, and response time.

## Features
- First Come First Serve (FCFS) scheduling
- Round Robin scheduling with user-defined time quantum
- Preemptive Priority Scheduling with optional aging
- Shortest Job First (SJF) scheduling
- Optimize mode to compare algorithms and choose the best performer
- Gantt chart-style execution timeline
- Average waiting time, turnaround time, and response time analysis
- Modular multi-file C structure using headers and source files

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
./jackfruit
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
