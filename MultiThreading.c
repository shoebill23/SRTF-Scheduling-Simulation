#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

//Crossplatform threading imports 
#ifdef _WIN32
#include <windows.h>
typedef CRITICAL_SECTION Mutex;
void init_mutex(Mutex* m) { InitializeCriticalSection(m); }
void lock_mutex(Mutex* m) { EnterCriticalSection(m); }
void unlock_mutex(Mutex* m) { LeaveCriticalSection(m); }
typedef DWORD (WINAPI *ThreadFunc)(LPVOID);
void create_thread(HANDLE* thread, ThreadFunc func, void* arg) {
    *thread = CreateThread(NULL, 0, func, arg, 0, NULL);
}
void join_thread(HANDLE thread) { WaitForSingleObject(thread, INFINITE); }
void sleep_ms(int ms) { Sleep(ms); }
#else
#include <pthread.h>
#include <unistd.h>
typedef pthread_mutex_t Mutex;
void init_mutex(Mutex* m) { pthread_mutex_init(m, NULL); }
void lock_mutex(Mutex* m) { pthread_mutex_lock(m); }
void unlock_mutex(Mutex* m) { pthread_mutex_unlock(m); }
typedef void* (*ThreadFunc)(void*);
void create_thread(pthread_t* thread, ThreadFunc func, void* arg) {
    pthread_create(thread, NULL, func, arg);
}
void join_thread(pthread_t thread) { pthread_join(thread, NULL); }
void sleep_ms(int ms) { usleep(ms * 1000); }
#endif

//Global variables
int n; 
int completed = 0;
int currentTime = 0;

//Initializing Process Struct 
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
    int inReadyQueue; // New flag to track if we added it to the main queue yet
};

struct Process* tempQueue;   // Holds initial user input to simulate actual processes entering over time
struct Process* processQueue; //Simulates the processes being loaded into the CPU
int activeCount = 0; //To keep track of active processes

Mutex mutex;

void* InputHandler(void* args) {
    while (completed < n) {
        
        lock_mutex(&mutex); // Lock before touching shared queues

        for (int i = 0; i < n; i++) {

            if (tempQueue[i].arrivalTime <= currentTime && tempQueue[i].inReadyQueue == 0) {
                
                // Copy to shared queue
                processQueue[activeCount] = tempQueue[i];
                strcpy(processQueue[activeCount].processState, "Ready");
                
                // Mark as added in tempQueue so we don't add it again
                tempQueue[i].inReadyQueue = 1;
                
                printf("[Time %d Process %d arrived -> ReadyQueue[%d]]\n", currentTime, tempQueue[i].processID, activeCount);
                
                activeCount++; // Increment active processes
            }
        }

        unlock_mutex(&mutex);

        // Sleep briefly to yield CPU
        sleep_ms(10); 
    }
    return NULL;
}

void* Scheduler(void* args) {
    int minRemainingTime;
    int shortestIndex;

    while (completed < n) {
        lock_mutex(&mutex); //lock shared queue

        minRemainingTime = INT_MAX;
        shortestIndex = -1;

        for(int i = 0; i < activeCount; i++){
            if(processQueue[i].remainingTime > 0) { //Checks if process is completed or not
                if(processQueue[i].remainingTime < minRemainingTime){ //Checks if new process has faster burst time than current process
                    minRemainingTime = processQueue[i].remainingTime;
                    shortestIndex = i;
                }
            }
        }

        if(shortestIndex != -1){ //Checks if there are processes to run
            if(processQueue[shortestIndex].startTime == -1){ //Checks if loaded process has been run before 
                processQueue[shortestIndex].startTime = currentTime; //Sets start time for process
                processQueue[shortestIndex].responseTime = currentTime - processQueue[shortestIndex].arrivalTime; //Calculates response time
            }

            processQueue[shortestIndex].remainingTime--; //Decrements remaining time
            
            if(processQueue[shortestIndex].remainingTime == 0){ //Checks if process is completed
                strcpy(processQueue[shortestIndex].processState, "Completed");
                processQueue[shortestIndex].completionTime = currentTime + 1;
                processQueue[shortestIndex].turnaroundTime = processQueue[shortestIndex].completionTime - processQueue[shortestIndex].arrivalTime; //calculates turnaround time
                processQueue[shortestIndex].waitingTime = processQueue[shortestIndex].turnaroundTime - processQueue[shortestIndex].burstTime; //calculates waiting time
                completed++;
            } else {
                strcpy(processQueue[shortestIndex].processState, "Running");
            }

            printf("%-10d P%-10d %-12s Remaining: %d\n", currentTime, processQueue[shortestIndex].processID, processQueue[shortestIndex].processState, processQueue[shortestIndex].remainingTime);
        } else {
            printf("%-10d [CPU Idle]\n", currentTime);
        }

        unlock_mutex(&mutex);

        // Sleep 1 second to simulate 1 time unit
        sleep_ms(1000); 
        currentTime++;
    }
    return NULL;
}

int main() {
    // Disable buffering for instant print output
    setbuf(stdout, NULL);

    printf("Enter number of Processes: "); //Getting user input for number of processes  
    scanf("%d", &n);

    tempQueue = malloc(n * sizeof(struct Process)); //Dynamically allocating memory for both queues 
    processQueue = malloc(n * sizeof(struct Process)); 

    // Get user input
    for (int i = 0; i < n; i++) {
        tempQueue[i].processID = i + 1;
        printf("Arrival time for P%d: ", i+1);
        scanf("%d", &tempQueue[i].arrivalTime);
        printf("Burst time for P%d: ", i+1);
        scanf("%d", &tempQueue[i].burstTime);

        tempQueue[i].remainingTime = tempQueue[i].burstTime;
        tempQueue[i].startTime = -1;
        tempQueue[i].inReadyQueue = 0; // Initialize flag
    }

    init_mutex(&mutex);

    printf("%-10s %-11s %-12s %s\n", "Time", "PID", "State", "Info");

#ifdef _WIN32
    HANDLE t1, t2;
    create_thread(&t1, (ThreadFunc)InputHandler, NULL);
    create_thread(&t2, (ThreadFunc)Scheduler, NULL);
    join_thread(t1);
    join_thread(t2);
#else
    pthread_t t1, t2;
    create_thread(&t1, InputHandler, NULL);
    create_thread(&t2, Scheduler, NULL);
    join_thread(t1);
    join_thread(t2);
#endif
    
    // Print final report
    printf("PID\tArrival\tBurst\tWait\tTurnaround\n");
    double totalWait = 0, totalTurn = 0;
    for(int i=0; i<activeCount; i++) {
        // Sort by PID for display (optional, skipping for simplicity)
        printf("%d\t%d\t%d\t%d\t%d\n", 
            processQueue[i].processID,
            processQueue[i].arrivalTime,
            processQueue[i].burstTime,
            processQueue[i].waitingTime,
            processQueue[i].turnaroundTime);
        totalWait += processQueue[i].waitingTime;
        totalTurn += processQueue[i].turnaroundTime;
    }
    printf("Avg Waiting Time: %.2f\n", totalWait/n);
    printf("Avg Turnaround Time: %.2f\n", totalTurn/n);

    free(tempQueue);
    free(processQueue); //freeing pointers 
    return 0;
}