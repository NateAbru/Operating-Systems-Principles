/* Author: Nathaniel Abreu
 * CS340 Project 3
 * Professor: John Svadlenka
 * To Run file, compile first with "g++ nathaniel_abreu_project3.cpp"
 * Then enter "./a.out"
 */

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
using namespace std;

/* Process object to be passed to each node data in the 
 * ready queue using linked-list in order to store multiple data values
 * for each process
 */
struct process
{
	std::string name;
	int priority;
	int burst;
	int arrival;
	int wait_time = 0;
	int turn_around_time = 0;
	int time_stamp = 0;
};

/* Node object structured as a singly list linked-list
 * containing the data which is a "process Object" type
 * and the link to the next node
 */
struct Node
{
	process data;
	struct Node *next;
};

/* Insertion function to insert a new node
 * into the head of the linked-list
 */
void head_insert(struct Node** head_ref, process new_data) 
{ 
    struct Node* new_node = new Node; 
    new_node->data = new_data; 
    new_node->next = (*head_ref); 
    (*head_ref) = new_node; 
} 

/* Insertion function to insert a new node
 * at the end of a linked-list
 */
void endQueue_insert(struct Node** head_ref, process new_data)  
{  
	/* new node creation */
    Node* new_node = new Node(); 
    Node *last = *head_ref;
    new_node->data = new_data;  
  
  	/* linking the new node to NULL
  	 * in order to replace the current last node's next
  	 * with the new node later in this function
  	 */
    new_node->next = NULL;  
  	
  	/* checking if linked-list is empty
  	 * just make the new node the head if so
  	 */
    if (*head_ref == NULL)  
    {  
        *head_ref = new_node;  
        return;  
    }  
  
    /* Go through entire linked-list
     * to get to the last node
     */
    while (last->next != NULL)  
        last = last->next;  
  
    /* insert the next node into the next
     * of the last node
     */
    last->next = new_node;  
    return;  
} 

/* function to remove the head node from the singly linked-list */ 
Node* remove_head(struct Node* head) {
	/* if there is no node to remove return nothing */
    if (head == NULL) return NULL; 
  
    /* move current head to next */
    Node* temp = head; 
    head = head->next; 
  
  	/* delete the old head and return the new list without the old head */
    delete temp; 
    return head; 
}

/* function to remove the last node in the singly linked-list */
Node* remove_tail(struct Node* head)
{
	/* if nothing in list return nothing */
	if (head == NULL) 
        return NULL; 
  
    /* if head is the only node it is also the last node
     * so then, delete head 
     */
    if (head->next == NULL) { 
        delete head; 
        return NULL; 
    } 
  
    /* get the seocond to last node */
    Node* second_last = head; 
    while (second_last->next->next != NULL) 
        second_last = second_last->next; 
  
    /* deleting last node and assigning NULL to its slot
     * return new list with deleted last node
     */
    delete (second_last->next); 
    second_last->next = NULL; 
    return head; 
}

/* function to print the Turn around time and 
 * wait time for each process completed 
 */
void print_process_details(struct Node* head_ref)
{
	Node* last = head_ref;
	/* go through entire link list and print info for each node */
	while (last != NULL)  
	{
		std::cout << last->data.name << ": Total Turnaround Time = " << last->data.turn_around_time 
		<< ", Total Wait time = " << last->data.wait_time << std::endl;
        last = last->next; 
	}
	return;
}

int main()
{
	/* head of linked list to be designated as the 
	 * ready queue
	 */
	Node* head = NULL;
	/* head of linked list to be designated as the 
	 * completed list of processes
	 */
	Node* process_complete_list = NULL;
	std::string input_line, P_n, P_r ="";

	/* New process variables to be used to store the read in data */
	int P_n_priority, P_n_burst, P_n_arrival, P_n_wait_time = 0;

	/* running process variables to store the data of the running process while not
	 * in the ready queue and in the running state
	 */
	int P_r_priority, P_r_burst, P_r_arrival, P_r_wait_time = 0; 
	int time_quantum = -50; /* Arbitary value to help set up the initial time quantum condition later */

	/* declaration of variable to open data file */
	ifstream iFile;

	/* loop that increments a time variable t by 1 from 0 to 96 
	 * inside this loop the data file is opended and each process 
	 * is handled and placed in the ready queue based on 
	 * priority and the scheuling alogorithm implemented.
	 * The scheduling algorithm is preemptive, priority based with Round-Robin scheduling for
	 * processes with the same priorty level using a time Quantum = 10
	 */
	for(int t = 0; t < 97; t++)
	{
		/* open data file */
		iFile.open("process_data.txt");

		/* run through data file while it has not reached the end of the file */
		while(!iFile.eof())
		{
			/* get each line of data in the data file */
			getline(iFile, input_line);
			if(!iFile) break;

			/* input the data into a buffer and assign
			 * the data. Every value after a space in each line 
			 * is one datum assigned to variables
			 */
			istringstream buffer(input_line);
			buffer >> P_n >> P_n_priority >> P_n_burst >> P_n_arrival;
		
			/* New process coming in according to arrival time */
			if(P_n_arrival == t)
			{
				/* assign the data of new process to Object variable
				 * to store all its contents
				 * and to be placed in the ready queue
				 */
				process new_process;
				new_process.name = P_n;
				new_process.priority = P_n_priority;
				new_process.burst = P_n_burst;
				new_process.arrival = P_n_arrival;
				new_process.wait_time = P_n_wait_time;
				new_process.time_stamp = t;
				endQueue_insert(&head, new_process);

				/* condition if there is no process currently in
				 * the running state, then enter the new process
				 * in the running state and remove from the ready queue
				 */
				if(P_r == "")
				{
					P_r = P_n;
					P_r_priority = P_n_priority;
					P_r_burst = P_n_burst;
					P_r_arrival = P_n_arrival;
					P_r_wait_time = P_n_wait_time;
					head = remove_head(head);
					std::cout << "Time=" << t << " " << P_r << std::endl;
				}

				/* else then if there is a process in the running state already
				 * and the new process has higher priority
				 * preempt current Process with the lower priority
				 * and put the new process in the running state
				 */
				else if(P_n_priority > P_r_priority)
				{
					/* checking first if the current running process also completed
					 * at the same time that the new process with higher priority came in
					 * so that the process doesnt get lost and moved back to the ready queue when
					 * it has actually already completed
					 */
					if(P_r_burst == 0)
					{
						process p_complete;
						p_complete.name = P_r;
						p_complete.priority = P_r_priority;
						p_complete.burst = P_r_burst;
						p_complete.arrival = P_r_arrival;
						p_complete.wait_time += P_r_wait_time;
						p_complete.turn_around_time = t - P_r_arrival;
						/* moving to the completed list */
						endQueue_insert(&process_complete_list, p_complete);			
	
						if(head != NULL)
						{
							P_r = head->data.name;
							P_r_priority = head->data.priority;
							P_r_burst = head->data.burst;
							P_r_arrival = head->data.arrival;
							P_r_wait_time = head->data.wait_time + (t - (head->data.time_stamp));
							head = remove_head(head);
						}
					}

					/* Print out the process names in the context switch for 
					 * higher priority process and preempted process
					 */
					std::cout << "Context Switch: " << "t=" << t << " ("<< P_n << ", " << P_r << ")" << std::endl;
					std::cout << "Time=" << t << " " << P_n << std::endl;

					/* insert current running process back to ready queue */ 
					head = remove_tail(head);
					process pSwitch;
					pSwitch.name = P_r;
					pSwitch.priority = P_r_priority;
					pSwitch.burst = P_r_burst;
					pSwitch.arrival = P_r_arrival;
					pSwitch.wait_time += P_r_wait_time;
					pSwitch.time_stamp = t;
					if((head != NULL) && (pSwitch.priority >= head->data.priority))
					{
						head_insert(&head, pSwitch);
					}
					else endQueue_insert(&head, pSwitch);

					/* place new process with higher priority 
					 * into the running state and assign time quantum
					 */
					P_r = P_n;
					P_r_priority = P_n_priority;
					P_r_burst = P_n_burst;
					P_r_arrival = P_n_arrival;
					P_r_wait_time = P_n_wait_time;
				}

				/* incoming process with same priority level
				 * as current process in the running state.
				 * scheduled using Round Robin Scheduling
				 */
				else if(P_n_priority == P_r_priority)
				{
					int temp = t - P_r_arrival;

					/* remove the new process from end of ready queue
					 * and insert it back into the head of the queue
					 */
					head = remove_tail(head);
					process pRobin;
					pRobin.name = P_n;
					pRobin.priority = P_n_priority;
					pRobin.burst = P_n_burst;
					pRobin.arrival = P_n_arrival;
					pRobin.wait_time = P_n_wait_time;
					pRobin.time_stamp = t;
					head_insert(&head, pRobin);

					if((time_quantum < -49) && (temp < 10))
					{
						time_quantum = 10 - temp;
					}
					else time_quantum = 10;
				}
			}
		}
		iFile.close();

		/* if Given Time quantum has expired 
		 * check if the running process has completed or not
		 * if completed assign next in queue
		 * if not then move back to ready queue
		 */
		if((time_quantum == 0) && (P_r_burst == 0))
		{ 			
			//process complete
			process p_complete;
			p_complete.name = P_r;
			p_complete.priority = P_r_priority;
			p_complete.burst = P_r_burst;
			p_complete.arrival = P_r_arrival;
			p_complete.wait_time += P_r_wait_time;
			p_complete.turn_around_time = t - P_r_arrival;
			/* moving to the completed list */
			endQueue_insert(&process_complete_list, p_complete);
			
			if(head != NULL)
			{
				P_r = head->data.name;
				P_r_priority = head->data.priority;
				P_r_burst = head->data.burst;
				P_r_arrival = head->data.arrival;
				P_r_wait_time = head->data.wait_time + (t - (head->data.time_stamp));
				head = remove_head(head);
				std::cout << "Time=" << t << " " << P_r << std::endl;
			}
		}
		/* process not complete 
		 * place back in ready queue and put next process in running state 
		 */ 
		else if(time_quantum == 0 && P_r_burst != 0)
		{
			
			process p_unfinished;
			p_unfinished.name = P_r;
			p_unfinished.priority = P_r_priority;
			p_unfinished.burst = P_r_burst;
			p_unfinished.arrival = P_r_arrival;
			p_unfinished.wait_time += P_r_wait_time;
			p_unfinished.time_stamp = t;
			
			if(head != NULL)
			{
				P_r = head->data.name;
				P_r_priority = head->data.priority;
				P_r_burst = head->data.burst;
				P_r_arrival = head->data.arrival;
				P_r_wait_time = head->data.wait_time + (t - (head->data.time_stamp));
				head = remove_head(head);
				time_quantum = 10;
				std::cout << "Time=" << t << " " << P_r << std::endl;
			}
			head_insert(&head, p_unfinished);
		}

		/* process has completed its burst time before the
		 * time quantum has expired
		 * if a time quantum was assigned
		 * Otherwise the process just completed its burst time
		 */
		else if(P_r_burst == 0)
		{
			process p_complete;
			p_complete.name = P_r;
			p_complete.priority = P_r_priority;
			p_complete.burst = P_r_burst;
			p_complete.arrival = P_r_arrival;
			p_complete.wait_time += P_r_wait_time;
			p_complete.turn_around_time = t - P_r_arrival;
			endQueue_insert(&process_complete_list, p_complete);
	
			if(head != NULL)
			{
				P_r = head->data.name;
				P_r_priority = head->data.priority;
				P_r_burst = head->data.burst;
				P_r_arrival = head->data.arrival;
				P_r_wait_time = head->data.wait_time + (t - (head->data.time_stamp));
				head = remove_head(head);
				std::cout << "Time=" << t << " " << P_r << std::endl;
			}
		}
		P_r_burst--;
		time_quantum--;
	}
	/* printing the Turnaround time and wait time for the processes that completed */
	print_process_details(process_complete_list);
}