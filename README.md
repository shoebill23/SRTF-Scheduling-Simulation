# CPU Scheduling Simulator (SRTF)

This project implements the Shortest Remaining Time First (SRTF) CPU scheduling algorithm using C. It provides two different architectural approaches to simulate process scheduling: a sequential Single-Threaded version and a concurrent Multi-Threaded version.

## Files Overview

### 1. SingleThreading.c

A standard, iterative implementation of the SRTF algorithm.

Logic: It runs a single simulation loop that increments time. Inside the loop, it checks for process arrivals and selects the process with the shortest remaining time to execute.

Use Case: Best for understanding the core algorithmic logic of SRTF without the complexity of synchronization.

### 2. MultiThreading.c

A more complex, realistic simulation using parallel threads to mimic an Operating System environment.

Logic: Implements a Producer-Consumer pattern.

Input Handler Thread (Producer): Simulates an I/O controller. It monitors the "world time" and moves processes from the input list to the CPU Ready Queue when their arrival time is reached.

Scheduler Thread (Consumer): Simulates the CPU. It constantly scans the Ready Queue to pick the shortest job, executes it, and manages context switching.

Synchronization: Uses Mutex locks to prevent race conditions when both threads attempt to access the shared Ready Queue simultaneously.

Cross-Platform: The code contains preprocessor directives (#ifdef _WIN32) to support both Windows API threads and POSIX pthreads (Linux/macOS) automatically.

Compilation & Usage

### Prerequisites

A C Compiler (GCC, Clang, or MSVC).

## How to Compile

### Linux / macOS

The multi-threaded version requires linking the pthread library.

### Compile Single Threaded
gcc SingleThreading.c -o single_scheduler

### Compile Multi Threaded
gcc MultiThreading.c -o multi_scheduler -lpthread


### Windows (MinGW/GCC)

The code detects Windows automatically and uses <windows.h>.

gcc SingleThreading.c -o single_scheduler.exe
gcc MultiThreading.c -o multi_scheduler.exe


### How to Run

Run the executable.

Enter the number of processes (e.g., 3).

For each process, enter the Arrival Time and Burst Time.

Example Input:
```
Enter number of Processes: 3
Arrival time for P1: 0
Burst time for P1: 5
Arrival time for P2: 1
Burst time for P2: 3
Arrival time for P3: 2
Burst time for P3: 1
```

### Output Metrics

Both programs calculate and display:

Turnaround Time: Completion Time - Arrival Time

Waiting Time: Turnaround Time - Burst Time

Response Time: Time of first execution - Arrival Time

Average Waiting & Turnaround Times
