# Paper

[Paper.pdf](https://github.com/ZFCrow/RoundRobinwithVaryingTimeQuantum/files/14969683/Paper.pdf)
# Acknowledgement 

This code implementation is based on the enhanced Round Robin scheduling algorithm as presented in [Paper.pdf]. The paper introduces an innovative approach that utilizes a dual queue system, improving upon the traditional Round Robin method. The original authors have provided two test cases to benchmark this algorithm against the classic Round Robin algorithm. Alongside these provided cases, I have developed additional test scenarios to conduct a broader comparison with other scheduling algorithms.
# Introduction

This algorithm is an improved variant of the traditional Round Robin scheduling method. It improves the standard approach by implementing a dual queue system, which assigns a more suitable time quantum based on the medium burst time requirements of the processes. Unlike Round Robin, where a fixed time quantum may not suit all types of processes with varying burst times, this proposed approach dynamically computes a “medium burst time” which essentially acts as a benchmark to sort the arrived processes into two distinct queues. The Light Task Queue(LTQ) and Heavy Task Queue(HTQ).
The term ‘medium’ is used as it strikes a balance between the two scenarios of handling an even and odd amount of processes. The algorithm uses the median burst time for an odd count of processes and the average for an even count. 
Upon populating both the Task Queues, the algorithm proceeds to calculate a new, queue-specific quantum time for each queue, immediately prior to the execution phase, ensuring that each queue’s time quantum is precisely tuned for the current processes it holds. The LTQ, which is meant for the lighter tasks, is processed first. If the LTQ is not entirely cleared after this round of execution, indicating there are tasks remaining with their leftover burst times, the algorithm will not proceed to the HTQ. Instead, it recalculates the quantum time for the leftover LTQ processes and proceeds on with executing the tasks from the LTQ. This cycle will then repeat itself until the LTQ depletes completely. This highlights the algorithm’s flexible nature, as it adjusts dynamically to the ongoing demands of the queues it created. The HTQ will then undergo the same process, with a new computed time quantum meant for the heavier tasks with longer burst times. 
This algorithm ensures that at every round of execution, the time quantum is computed optimally for its set of tasks that is about to be executed, whether it is a new round of tasks that just got sorted into the queues or the leftover tasks after initial execution. With this, it carefully reduces the number of context switches needed while preventing a particular process from hogging the CPU, reducing overall waiting times as well.


## Implementation Versions
There are two distinct versions of the implementation available:

paper3testcaseversion.c: This version is designed to automatically loop through test cases provided in testcase.csv up to the number specified within the code. It then outputs the results to output.csv for easy analysis and comparison.

paper3finalversion.c: This version offers more interactivity, prompting users to choose between entering process data manually or selecting one of the test cases from testcase.csv. This flexibility allows for customized input to observe how the algorithm handles various process loads.

Both versions serve to illustrate the algorithm's performance, with one providing automated test case processing and the other allowing for manual input and single test case execution.
