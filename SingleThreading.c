#include <stdio.h>
#include <string.h>
#include <limits.h>

struct Process {
    int processID;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int completionTime;
    int waitingTime;
    int turnaroundTime;
    char processState[10];
    int responseTime;
    int startTime;
};

int main(){
    int n; 
    int completed = 0;
    int currentTime = 0;
    int shortestIndex; 
    int minRemainingTime;
    int totalTurnaround = 0, totalWait = 0;
    float avgTurnaround = 0, avgWait = 0;

    printf("Enter number of Processes (1-10): ");
    scanf("%d", &n);

    struct Process processQueue[n];

    for(int i = 0; i < n; i++){
        processQueue[i].processID = i + 1;
        
        printf("Enter arrival time for Process %d: ", i+1);
        scanf("%d", &processQueue[i].arrivalTime);

        printf("Enter burst time for Process %d: ", i+1);
        scanf("%d", &processQueue[i].burstTime);

        processQueue[i].remainingTime = processQueue[i].burstTime;
        processQueue[i].completionTime = 0;
        processQueue[i].turnaroundTime = 0;
        processQueue[i].waitingTime = 0;
        processQueue[i].startTime = -1;
        processQueue[i].responseTime = 0;
        strcpy(processQueue[i].processState, "Ready");
    }

    // UPDATED: Use fixed width formatting instead of tabs for perfect alignment
    // %-10s means "reserve 10 characters of space, left aligned"
    printf("\n%-10s%-12s%-12s%s\n", "Time", "Process ID", "Status", "Remaining Time");

    while (completed != n){
        
        // Print "Ready" status when a process arrives
        for(int i = 0; i < n; i++){
            if(processQueue[i].arrivalTime == currentTime){
                printf("%-10d P%-10d %-12s %d\n", 
                       currentTime, 
                       processQueue[i].processID, 
                       processQueue[i].processState, 
                       processQueue[i].remainingTime);
            }
        }

        shortestIndex = -1;
        minRemainingTime = INT_MAX;

        for(int i = 0; i < n; i++){
            if(processQueue[i].arrivalTime <= currentTime && processQueue[i].remainingTime > 0){
                if(processQueue[i].remainingTime < minRemainingTime){
                    minRemainingTime = processQueue[i].remainingTime;
                    shortestIndex = i;
                }
            }
        }

        if(shortestIndex != -1){
            if(processQueue[shortestIndex].startTime == -1){
                processQueue[shortestIndex].startTime = currentTime;
                processQueue[shortestIndex].responseTime = currentTime - processQueue[shortestIndex].arrivalTime;
            }

            processQueue[shortestIndex].remainingTime--;
            
            if(processQueue[shortestIndex].remainingTime == 0){
                strcpy(processQueue[shortestIndex].processState, "Completed");
                processQueue[shortestIndex].completionTime = currentTime + 1;
                processQueue[shortestIndex].turnaroundTime = processQueue[shortestIndex].completionTime - processQueue[shortestIndex].arrivalTime;
                processQueue[shortestIndex].waitingTime = processQueue[shortestIndex].turnaroundTime - processQueue[shortestIndex].burstTime;
                completed++;
            } else {
                strcpy(processQueue[shortestIndex].processState, "Running");
            }

            // Print "Running" or "Completed" status aligned
            printf("%-10d P%-10d %-12s %d\n", 
                   currentTime, 
                   processQueue[shortestIndex].processID, 
                   processQueue[shortestIndex].processState, 
                   processQueue[shortestIndex].remainingTime);
        }

        currentTime++;
    }

    printf("\nSRTF Performance Results:\n");

    for(int i = 0; i < n; i++){
        printf("Process %d: Turnaround = %d, Waiting = %d, Response = %d\n", 
               processQueue[i].processID, 
               processQueue[i].turnaroundTime, 
               processQueue[i].waitingTime, 
               processQueue[i].responseTime);

        totalTurnaround += processQueue[i].turnaroundTime;
        totalWait += processQueue[i].waitingTime;
    }

    avgTurnaround = (float)totalTurnaround / n;
    avgWait = (float)totalWait / n;

    printf("\nAverage Turnaround Time = %.2f\n", avgTurnaround);
    printf("Average Waiting Time = %.2f\n", avgWait);

    return 0;
}