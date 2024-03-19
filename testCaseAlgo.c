//goal is to implement a cpu scheduling algorithm

//a struct to represent a process containing the process id, arrival time, burst time, waiting time, turnaround time

// find a medium burst time 

// separate the processes into two queues, one for processes with burst time less than the medium burst time (light tasks queue)
// the other for processes with burst time greater than the medium burst time (heavy tasks queue)

//quantum time for the light tasks queue eqautes to the medium burst time in that queue
//if the no. of process is even , then the quantum time equates to the average burst time of the processes in that queue


//in that queue, the process with the shortest burst time is executed first with the give quantum time
//after the quantum time, the process is moved to the end of the queue with the left over burst time
//wait time for each process is calculated as the sum of the burst time of all the processes that arrived before it, if the burst time of the process is less than the quantum time, thats the wait time for the process



#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

int currentTime; //current ms in the system 
double medium_burst_time; //medium burst time of the processes in the ready queue 
int no_of_processes = 0 ;
int testcase = 1; //testcase number 
int contextSwitches = 0; //number of context switches


typedef struct process{
    int process_id;
    int arrival_time;
    int burst_time;
    int waiting_time;
    int response_time;
    int turnaround_time; 
    int total_executed_time; //total time the process has been executed 

    struct process *next; 
}Process; 


//*4 queues are implemented, one for the ready queue, one for the light tasks queue, one for the heavy tasks queue and one for the finished queue
typedef struct queue {
    Process *frontprocesses; // pointer to the front of the queue
    Process *rearprocesses; // pointer to the rear of the queue
    int size; // size of the queue
}Queue; 


void addtoReadyQueue(Queue *queue, Process *process){ 
    
    if(queue->size == 0){
        queue->frontprocesses = process;
        queue->rearprocesses = process;
        queue->size++;
    }else{
        queue->rearprocesses->next = process;
        queue->rearprocesses = process;
        queue->size++;
    }

    //check if the process has a next process, if yes unlink the next process
    if(process->next != NULL){
        process->next = NULL;
    }
}
void dequeue(Queue *queue, Process *process){
   

    if(queue->size == 0){
        printf("The queue is empty\n");
    }else{
        Process *temp = queue->frontprocesses;
        Process *prev = NULL; 

        while(temp != NULL){
            if(temp->process_id == process->process_id){

                if(prev == NULL){
                    queue->frontprocesses = temp->next;
                }else{
                    prev->next = temp->next;
                }
                queue->size--;
                break;
            }
            prev = temp;
            temp = temp->next;
        }
    }
}

//exeucuting the process with the given quantum time 
void executeProcessFromQueue(Queue *queue, int quantumTime, Queue *finishedQueue){
    //if burst time of the process is less than the quantum time, then the process is executed
    //else the process is executed for the quantum time and the burst time is updated
    //this wont affect the waiting time of the process as the next time the process is executed, the waiting time is updated
    Process *temp = queue->frontprocesses;
    while(temp != NULL){
        Process *nextProcess = temp->next;

        //response time check 
        if(temp->response_time == -1){
            temp->response_time = currentTime - temp->arrival_time;
        }


        if(temp->burst_time <= quantumTime){
            
            currentTime += temp->burst_time;
            temp->total_executed_time += temp->burst_time; //update the total time the process has been executed
            temp->turnaround_time = currentTime - temp->arrival_time; 
           // temp->waiting_time = temp->turnaround_time - temp->burst_time; //this would be inaccurate if the process is executed more than once, so the waiting time is calculated after the process is executed
            temp->waiting_time = currentTime - temp->arrival_time - temp->total_executed_time;
            printf("The process with process id %d has been executed\n", temp->process_id);
            printf("The waiting time for the process is: %d\n", temp->waiting_time);
            printf("The turnaround time for the process is: %d\n", temp->turnaround_time);
            temp -> burst_time = 0;

            dequeue(queue, temp);
            addtoReadyQueue(finishedQueue, temp);
            
        }
        else{
            currentTime += quantumTime;
            temp->burst_time -= quantumTime;
            temp->total_executed_time += quantumTime; //update the total time the process has been executed

        }
        contextSwitches++; //but this wont check if the process executed is the one from before 
        temp = nextProcess; //update temp to the next process 
    }
}


void printReadyQueue(Queue *queue,int status){
    Process *temp = queue->frontprocesses;
    if (status){
        printf("Process ID\tArrival Time\tBurst Time\n");
    }
    else{
        printf("Process ID\tArrival Time\tBurst Time\tWaiting Time\tResponse Time\tTurnaround Time\n");
    }
    while(temp != NULL){
        if(status){
     
        printf("%d\t\t%d\t\t%d\n", temp->process_id, temp->arrival_time, temp->burst_time);
        }else{
            //print the waiting time and turnaround time
            printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", temp->process_id, temp->arrival_time, temp->burst_time, temp->waiting_time, temp->response_time, temp->turnaround_time);
        }
        temp = temp->next;
    }
}

//calulcate the medium burst time with the processes in the ready queue. 
void calculateMediumBurstTime(Queue *queue){
    //if number of processes is even, then the medium burst time is the average burst time of the processes in the queue
    //if number of processes is odd, then the medium burst time is the burst time of the process in the middle of the queue
    Process *temp = queue->frontprocesses;
    int total_burst_time = 0;
    int numberofprocesses = 0;
    while(temp != NULL){
        total_burst_time += temp->burst_time;
        numberofprocesses++;
        temp = temp->next;
    }
    if(numberofprocesses % 2 == 0){
        printf("number of processes is even at %d\n", numberofprocesses);
        medium_burst_time = total_burst_time / numberofprocesses;

    }else{
        temp = queue->frontprocesses;
        for(int i = 0; i < numberofprocesses/2; i++){
            temp = temp->next;
        }
        medium_burst_time = temp->burst_time;
    }
    printf("The medium burst time is: %f\n", medium_burst_time);
}


// sort the whole queue base off the burst time of the processes 
void sortQueue(Queue *queue){
    if (queue->size <= 1){
        return;
    }

    Process **pptr = &queue->frontprocesses; // Pointer to the pointer to the first node / memory address of the memory address variable of the first node
    int swapped;
    do {
        swapped = 0;
        while (*pptr && (*pptr)->next) { // While there are at least two nodes to compare(memory address of the first node and the memory address of the next node)
            if ((*pptr)->burst_time > (*pptr)->next->burst_time) {
                // Swap nodes
                Process *temp = *pptr; //memory address of the first node
                *pptr = (*pptr)->next; //modifying the pointer to point to the next node, so the first node now becomes the second node
                temp->next = (*pptr)->next; // this original head node now points to the 3rd node in the list
                (*pptr)->next = temp; // the second node, which became the first node, now points to the original first node

                swapped = 1;
            }
            pptr = &(*pptr)->next; 
        }
        pptr = &queue->frontprocesses;
    } while (swapped);

    //print out the sorted queue
    printf("The sorted queue\n");
    printReadyQueue(queue,1); //if i dont return the queue, the queue is not sorted
}
//sort the whole queue base off arrival time of the processes
void sortQueueArrivalTime(Queue *queue){
    if (queue->size <= 1){
        return;
    }

    Process **pptr = &queue->frontprocesses; // Pointer to the pointer to the first node / memory address of the memory address variable of the first node
    int swapped;
    do {
        swapped = 0;
        while (*pptr && (*pptr)->next) { // While there are at least two nodes to compare(memory address of the first node and the memory address of the next node)
            if ((*pptr)->arrival_time > (*pptr)->next->arrival_time) {
                // Swap nodes
                Process *temp = *pptr; //memory address of the first node
                *pptr = (*pptr)->next; //modifying the pointer to point to the next node, so the first node now becomes the second node
                temp->next = (*pptr)->next; // this original head node now points to the 3rd node in the list
                (*pptr)->next = temp; // the second node, which became the first node, now points to the original first node

                swapped = 1;
            }
            pptr = &(*pptr)->next; 
        }
        pptr = &queue->frontprocesses;
    } while (swapped);

    //print out the sorted queue
    printf("The sorted queue with Arrival Time\n");
    printReadyQueue(queue,1); 
}

void sortintoDoubleQueue(Queue *queue, Queue *lightTasksQueue, Queue *heavyTasksQueue){
    Process *temp = queue->frontprocesses;
    while(temp != NULL){
        printf("The burst time is: %d\n", temp->burst_time);
        Process *nextProcess = temp->next; // Save the next process


        //remove the process from the ready queue
        dequeue(queue, temp);

        //add to respective Queue base off medium time
        if(temp->burst_time <= medium_burst_time){
            printf("The process is added to the light tasks queue\n");
            addtoReadyQueue(lightTasksQueue, temp);
        }else{
            printf("The process is added to the heavy tasks queue\n");
            addtoReadyQueue(heavyTasksQueue, temp);
        }
        temp = nextProcess; // Update temp to the saved next process
    }


}

bool isProcessInQueue(Queue *queue, int processId) {
    Process *current = queue->frontprocesses;

    while (current != NULL) {
        if (current->process_id == processId) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void calculateAverageTime(Queue *queue){
    Process *temp = queue->frontprocesses;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int total_response_time = 0;
    while(temp != NULL){
        total_waiting_time += temp->waiting_time;
        total_turnaround_time += temp->turnaround_time;
        total_response_time += temp->response_time;
        temp = temp->next;
    }

    float average_waiting_time = (float)total_waiting_time/queue->size;
    float average_turnaround_time = (float)total_turnaround_time/queue->size; 
    float average_response_time = (float)total_response_time/queue->size;

    printf("The average waiting time is: %.2f\n", average_waiting_time);
    printf("The average turnaround time is: %.2f\n", average_turnaround_time);
    printf("The average response time is: %.2f\n", average_response_time);
    
    //print context switch 
    printf("The number of context switches is: %d\n", contextSwitches-1); //minus 1 because the last process does not have a context switch 

    //write it to a excel file, append the data to the file, 
    FILE *file = fopen("output.csv", "a");
    fprintf(file, "%d,%.2f,%.2f,%.2f,%d\n", testcase, average_waiting_time, average_turnaround_time,average_response_time, contextSwitches-1);
    fclose(file);
}

void queueInit (Queue *queue){
    queue->frontprocesses = NULL;
    queue->rearprocesses = NULL;
    queue->size = 0;
}

Process* readProcessesfromCSV(int testcase){
    //read the processes from a csv file
    // column names Test Case,Process ID,Arrival Time,Burst Time
    //read the processes from the csv file base on the testcase Number, populate the number of rows found into the no_of_processes variable 
    FILE *file = fopen("testcase.csv", "r");
    //skip the first line
    char line[1024];
    fgets(line, 1024, file);

    while (fgets(line, 1024, file) != NULL){
        char *token;
        token = strtok(line, ",");
        int current_testcase = atoi(token);
        if(current_testcase == testcase){
            no_of_processes++;

        }
    }
    Process *processes = (Process *)malloc(no_of_processes * sizeof(Process)); //allocate memory for the processes
    //populate the processes array with the processes from the csv file 
    int i = 0;
    fseek(file, 0, SEEK_SET);
    fgets(line, 1024, file);
    while (fgets(line, 1024, file) != NULL){
        char *token;
        token = strtok(line, ",");
        int current_testcase = atoi(token);
        if(current_testcase == testcase){
            token = strtok(NULL, ",") + 1; //remove the P
            processes[i].process_id = atoi(token);
            token = strtok(NULL, ",");
            processes[i].arrival_time = atoi(token);
            token = strtok(NULL, ",");
            processes[i].burst_time = atoi(token);
            
            i++;
        }

    }
    
    fclose(file);
    return processes;
}

int main(){
    int no_of_processes_added = 0 ;
    int no_of_processes_left;
    Process *processes;
    while (testcase<=102){

    
        //reset the current time to 0
        currentTime = 0;      
        contextSwitches = 0;   
        no_of_processes = 0; 
        no_of_processes_added = 0;

        processes = readProcessesfromCSV(testcase);
        printf("The number of processes is: %d\n", no_of_processes);
        for(int i = 0; i < no_of_processes; i++){
            printf("The process id is: %d\n", processes[i].process_id);
            printf("The arrival time is: %d\n", processes[i].arrival_time);
            printf("The burst time is: %d\n", processes[i].burst_time);

            //initialize the waiting time,turnaround, total executed to 0 and next to null
            processes[i].waiting_time = 0;
            processes[i].turnaround_time = 0;
            processes[i].total_executed_time = 0;
            processes[i].response_time = -1;
            processes[i].next = NULL;
        }

        no_of_processes_left = no_of_processes; 


        // print out the processes and their details in a table
        printf("Process ID\tArrival Time\tBurst Time\n"); 
        for(int i = 0; i < no_of_processes; i++){
            printf("%d\t\t%d\t\t%d\n", processes[i].process_id, processes[i].arrival_time, processes[i].burst_time);
        }


        //looks up for currentTime and adds the processes to the ready queue 

        //Arrival Queue
        Queue *arrivalQueue = (Queue *)malloc(sizeof(Queue));
        queueInit(arrivalQueue); 

        //readyQueue
        Queue *queue = (Queue *)malloc(sizeof(Queue)); 
        queueInit(queue);

        //create a finishedqueue to store the processes that have been executed
        Queue *finishedQueue = (Queue *)malloc(sizeof(Queue));
        queueInit(finishedQueue);

        //create the light tasks queue
        Queue *lightTasksQueue = (Queue *)malloc(sizeof(Queue));
        queueInit(lightTasksQueue);

        //create the heavy tasks queue
        Queue *heavyTasksQueue = (Queue *)malloc(sizeof(Queue));
        queueInit(heavyTasksQueue);


        //add all proccesses to the arrival queue
        for(int i = 0; i < no_of_processes; i++){
            addtoReadyQueue(arrivalQueue, &processes[i]);
        }
        //sort the arrival queue base off the arrival time of the processes
        sortQueueArrivalTime(arrivalQueue);

        while(arrivalQueue->size > 0){
            printf("The current time is: %d\n", currentTime);
            //loop through the queue, if the first one arrival time is more than the current time, change the current time to the arrival time of the first process
            //else add the process to the ready queue
            Process *temp = arrivalQueue->frontprocesses;
            while(temp != NULL){
                Process *nextProcess = temp->next;
                if(temp->arrival_time > currentTime){

                    if (no_of_processes_added == 0){
                        //first one so i will just modify the current time and then add the process to the ready queue
                        currentTime = temp->arrival_time;
                        dequeue(arrivalQueue, temp);
                        addtoReadyQueue(queue, temp);
                        no_of_processes_added++;
                        no_of_processes_left--;
                        }
                    else{
                        // it means something has been added to the ready queue and i reached a process with a higher arrival time
                        //no need check remaining processes, just break as the rest definitely have a higher arrival time
                        break;
                }
                }
                else{
                    //temp is less than or equal to the current time, so add it to the ready queue
                    dequeue(arrivalQueue, temp);
                    addtoReadyQueue(queue, temp);
                    no_of_processes_added++;
                    no_of_processes_left--;
                }

                temp = nextProcess;
            }

            no_of_processes_left -= no_of_processes_added;
            no_of_processes_added = 0;



            printf("The ready queue\n");
            printReadyQueue(queue,1);

            // with the ready queue, we can now implement the scheduling algorithm, we separate the processes 
            // into two queues, one for processes with burst time less than the medium burst time (light tasks queue)
            // the other for processes with burst time greater than the medium burst time (heavy tasks queue)
            //sort
            sortQueue(queue);
            calculateMediumBurstTime(queue);

            //separate the processes into the light and heavy tasks queue, remove it from the ready queue 
            sortintoDoubleQueue(queue, lightTasksQueue, heavyTasksQueue);
            int count = 1;

            //a loop to execute the processes in the light tasks queue and the heavy tasks queue 
            while (lightTasksQueue->size > 0 || heavyTasksQueue->size > 0){
                if(lightTasksQueue->size > 0){
                    calculateMediumBurstTime(lightTasksQueue);
                    executeProcessFromQueue(lightTasksQueue, medium_burst_time, finishedQueue);
                    printf("execution %d\n", count);
                    printf("The finished queue\n");
                    printReadyQueue(finishedQueue,0);
                    printf("The light tasks queue\n");
                    printReadyQueue(lightTasksQueue,0);
                    count++;
                    //if any process is left in the light tasks queue, recalculate the medium burst time and execute the processes in the light tasks queue
                    if(lightTasksQueue->size > 0){
                        continue; // this will execute the next loop
                    }
                }
                else{
                    calculateMediumBurstTime(heavyTasksQueue);
                    executeProcessFromQueue(heavyTasksQueue, medium_burst_time, finishedQueue);
                    printf("execution %d\n", count);
                    printf("The finished queue\n");
                    printReadyQueue(finishedQueue,0);
                    printf("The heavy tasks queue\n");
                    printReadyQueue(heavyTasksQueue,0);
                    count++;
                    if (heavyTasksQueue->size > 0)
                    {
                        continue;
                    }
                    
                }
            }
           


        }
    calculateAverageTime(finishedQueue);
    testcase++; 
    //free 
    free(arrivalQueue); 
    free(queue); 
    free(finishedQueue);
    free(lightTasksQueue);
    free(heavyTasksQueue);
    free(processes);
    
    }
}



