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

int currentTime; //current ms in the system 
double medium_burst_time; //medium burst time of the processes in the ready queue 

typedef struct process{
    int process_id;
    int arrival_time;
    int burst_time;
    int waiting_time;
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
        temp = nextProcess; //update temp to the next process 
    }
}


void printReadyQueue(Queue *queue,int status){
    Process *temp = queue->frontprocesses;
    if (status){
        printf("Process ID\tArrival Time\tBurst Time\n");
    }
    else{
        printf("Process ID\tArrival Time\tBurst Time\tWaiting Time\tTurnaround Time\n");
    }
    while(temp != NULL){
        if(status){
     
        printf("%d\t\t%d\t\t%d\n", temp->process_id, temp->arrival_time, temp->burst_time);
        }else{
            //print the waiting time and turnaround time
            printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\n", temp->process_id, temp->arrival_time, temp->burst_time, temp->waiting_time, temp->turnaround_time);
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

    // printf("The ready queue\n");
    // printReadyQueue(queue);
    // printf("The light tasks queue\n");
    // printReadyQueue(lightTasksQueue);
    // printf("The heavy tasks queue\n");
    // printReadyQueue(heavyTasksQueue);

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
    while(temp != NULL){
        total_waiting_time += temp->waiting_time;
        total_turnaround_time += temp->turnaround_time;
        temp = temp->next;
    }
    printf("The average waiting time is: %f\n", (float)total_waiting_time/queue->size);
    printf("The average turnaround time is: %f\n", (float)total_turnaround_time/queue->size);
}

void queueInit (Queue *queue){
    queue->frontprocesses = NULL;
    queue->rearprocesses = NULL;
    queue->size = 0;
}

int main(){
        //allow user to create the processes with a given arrival time and burst time 

        int no_of_processes;
        int no_of_processes_added;
        int no_of_processes_left;
        //!
        // printf("Enter the number of processes: ");
        // scanf("%d", &no_of_processes);

        // process *processes = (process *)malloc(no_of_processes * sizeof(process)); //allocate memory for the processes 

        // for(int i = 0; i < no_of_processes; i++){
        //     printf("Enter the arrival time for process %d: ", i+1);
        //     scanf("%d", &processes[i].arrival_time);
        //     printf("Enter the burst time for process %d: ", i+1);
        //     scanf("%d", &processes[i].burst_time);
        //     processes[i].process_id = i+1;
        // } 

        //!
        //!hardcoded the processes for now , all arrival times are 0, burst time = 15, 32, 10 ,26, 20
        //! prof suggested to put the test data in a file and read from the file
        no_of_processes = 5;
        no_of_processes_added = 0;
        no_of_processes_left = no_of_processes; 
        Process *processes = (Process *)malloc(no_of_processes * sizeof(Process)); //allocate memory for the processes
        // processes[0].arrival_time = 0;
        // processes[0].burst_time = 15;
        // processes[0].process_id = 1;
        // processes[0].next = NULL;
        // processes[1].arrival_time = 0;
        // processes[1].burst_time = 32;
        // processes[1].process_id = 2;
        // processes[1].next = NULL;
        // processes[2].arrival_time = 0;
        // processes[2].burst_time = 10;
        // processes[2].process_id = 3;
        // processes[2].next = NULL;
        // processes[3].arrival_time = 0;
        // processes[3].burst_time = 26;
        // processes[3].process_id = 4;
        // processes[3].next = NULL;
        // processes[4].arrival_time = 0;
        // processes[4].burst_time = 20;
        // processes[4].process_id = 5;
        // processes[4].next = NULL;



        // processes[5].arrival_time = 30;
        // processes[5].burst_time = 20;
        // processes[5].process_id = 6;
        // processes[5].next = NULL;

        processes[0].arrival_time = 0;
        processes[0].burst_time = 7;
        processes[0].process_id = 1;
        processes[1].next = NULL;
        processes[1].arrival_time = 4;
        processes[1].burst_time = 25;
        processes[1].process_id = 2;
        processes[1].next = NULL;
        processes[2].arrival_time = 10;
        processes[2].burst_time = 5;
        processes[2].process_id = 3;
        processes[2].next = NULL;
        processes[3].arrival_time = 15;
        processes[3].burst_time = 36;
        processes[3].process_id = 4;
        processes[3].next = NULL;
        processes[4].arrival_time = 17;
        processes[4].burst_time = 18;
        processes[4].process_id = 5;
        processes[4].next = NULL;
        //!


        // print out the processes and their details in a table
        printf("Process ID\tArrival Time\tBurst Time\n"); 
        for(int i = 0; i < no_of_processes; i++){
            printf("%d\t\t%d\t\t%d\n", processes[i].process_id, processes[i].arrival_time, processes[i].burst_time);
        }


        //looks up for currentTime and adds the processes to the ready queue 

        
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

        //this whole thing needs to be a loop so i can check for the processes that have arrived at the current time
      //  while (true){
        while(no_of_processes_left > 0){
            // if (no_of_processes_left == 0){
            //     break;
            // }else {
            //     printf("theres still processes\n");
            //     printf("numer of processes: %d\n", no_of_processes_left);
            //     //sleep(5);
            // }



            printf("The current time is: %d\n", currentTime);
            for(int i = 0; i < no_of_processes; i++){

                if(processes[i].arrival_time <= currentTime){
                    //skip it if the process is already in the finished queue
                    if (isProcessInQueue(finishedQueue, processes[i].process_id)){
                        continue;
                    }
                    printf ("The process with process id %d has arrived\n", processes[i].process_id);
                    addtoReadyQueue(queue, &processes[i]); //this is still pointing to the processes array
                    printf("The process with process id %d has been added to the ready queue\n", processes[i].process_id);
                    no_of_processes_added++;
                }
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
}



