/* Author: Nathaniel Abreu
 * CS340 Project 4
 * Professor: John Svadlenka
 * To Run file, compile first with "g++ allocator_NAbreu.cpp"
 * Then enter "./a.out 1048576"
 * 1048576 represents the 1mb memory given to the program in bytes
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <string.h> 
using namespace std;

/* Struct object representing the processes and its data */
struct process
{
	std::string name;
	int start_byte;
	int end_byte;
	int memory_size;
	std::string p_status;
	std::string attribute;
};

/* Node object structured as a doubly linked-list
 * containing the data which is a "process Object" type
 * and the link to the next node and previous node
 */
struct Node
{
	process data;
	struct Node *next;
	struct Node *prev;
};

/* Insertion function to insert a new node into the head of the linked-list */
void head_insert(struct Node** head_ref, process new_data) 
{ 
  	Node* new_node = new Node(); /* new node creation */
    new_node->data = new_data; /* passing the new data into new node */

    /* connecting new node to old head */
    new_node->next = (*head_ref); 
    new_node->prev = NULL; 
    if ((*head_ref) != NULL) (*head_ref)->prev = new_node; 
  
    (*head_ref) = new_node; /* make the new node the head */
    return;
} 

/* Insertion function to insert a new node at the end of a linked-list */
void endQueue_insert(struct Node** head_ref, process new_data)  
{   
    Node* new_node = new Node(); /* new node creation */
    Node* last = *head_ref; /* reference node to interate through list */
    new_node->data = new_data; /* passing the new data into new node */
    new_node->next = NULL; /* make new node point to NULL */
  
    /* check if list is empty to make new node the head */
    if (*head_ref == NULL) 
    { 
        new_node->prev = NULL; 
        *head_ref = new_node; 
        return; 
    } 
    /* else if list is not empty find last node */
    while (last->next != NULL) last = last->next; 
  
    last->next = new_node; /* make next of the last node the new node */
    new_node->prev = last; /* make the last node the prev of the new node */
    return;
} 
/* Insertion function to insert a new node between two nodes */
void insertAfter(Node* prev_node, process new_data)  
{  
    /* check if the given prev_node is NULL */
    if (prev_node == NULL)  
    {  
        std::cout << "No previous node";  
        return;  
    }  
  
    Node* new_node = new Node(); /* New node creation */
    new_node->data = new_data; /* put new data in new node */
    new_node->next = prev_node->next; /* make the next of the new node the next of the prev node */
    prev_node->next = new_node; /* make the new node the next of the prev node */ 
    new_node->prev = prev_node; /* make prev of the new node, the prev node */
  
    /* Change previous of new_node's next node */
    if (new_node->next != NULL) new_node->next->prev = new_node;  
}  

/* print out function for memory status */
void stat_func(struct Node* head_ref, int end_num)
{
	Node* last = head_ref; /* Node reference to head to iterate through list */
	while(last != NULL)
	{
		/* print out conditions */
		if(last->data.end_byte == end_num - 1)
		{
			std::cout << "Addresses[" << last->data.start_byte << " : " << "END] ";
		}
		else
		{
			std::cout << "Addresses[" << last->data.start_byte << " : " << last->data.end_byte << "] ";
		}
		if(last->data.p_status == "Released") 
		{
			std::cout << "Free" << std::endl;
			std::cout << "Block Size: " << last->data.memory_size << " Bytes" << std::endl;
			std::cout << std::endl;
		}
		if(last->data.p_status == "Active") 
		{
			std::cout << "Process " << last->data.name << std::endl;
			std::cout << "Block Size: " << last->data.memory_size << " Bytes" << std::endl;
			std::cout << std::endl;
		}
		last = last->next;
	}
	return;
}
/* function for releasing a process and freeing up memory */
int released_func(struct Node* head_ref, std::string process)
{
	Node* last = head_ref; /* Node reference to head to iterate through list */
	int memory_released = 0;
	while(last != NULL)
	{
		if(last->data.name == process)
		{
			last->data.p_status = "Released";
			last->data.name = "Free";
			memory_released += last->data.memory_size;
		}
		last = last->next;
	}
	return memory_released;
}
/* main function */
int main(int argc, char *argv[])
{
	char *s = argv[1]; /* variable to store main parameter argument which holds total memory amount */
	int total_memory = 0; /* initialization of variable to store the total memory after conversion to int */
	for(int i = 0; i < strlen(argv[1]); i++)
	{
		total_memory = (total_memory * 10) + (s[i] - '0');
	}

	int available_memory = total_memory; /* initialization of variable to keep track of remaining memory */
	Node* memory = NULL; /* Declaration of the pointer to be used for the memory stack using DLL */

	/* Initialization of the memory stack */
	process start_memory_stack;
	start_memory_stack.name = "Free";
	start_memory_stack.start_byte = 0;
	start_memory_stack.end_byte = total_memory - 1;
	start_memory_stack.memory_size = total_memory;
	start_memory_stack.p_status = "Released";
	start_memory_stack.attribute = "End";
	endQueue_insert(&memory, start_memory_stack);

	std::string user_input, cmand, p_name, strategy; /* declaration of variables to store user input */
	int byte_num; /* declaration of variable to store byte size of incoming process */ 
	std::cout << std::endl;
	//std::cout << "Memory : " << available_memory << " Bytes" << std::endl;
	std::cout << "Enter: ('RQ', 'RL', 'C', 'STAT', 'QUIT')" << std::endl; /* Menu selection */
	std::cout << "allocator>"; /* Input line where user types desired command request*/
	std::getline(std::cin, user_input); /* User input based on the desired command request*/

	while((user_input != "exit") && (user_input != "quit") && (user_input != "EXIT") && (user_input != "QUIT"))
	{
		/* if user enters Request new process command */
		if(((user_input[0] == 'R') || (user_input[0] == 'r')) && ((user_input[1] == 'Q') || (user_input[1] == 'q')))
		{
			istringstream buffer(user_input); /* input user input to buffer to extract values after space */
			buffer >> cmand >> p_name >> byte_num >> strategy; /* insert the values to the variables declared earlier */

			/* check if there is enough total memory remaining to take in new process */
			if(available_memory < byte_num) 
			{
				std::cout << "NOT ENOUGH MEMORY TO PROCESS REQUEST" << std::endl;
			}
			else /* if there is enough memory, use best fit strategy to place the new process in a memory slot avaible */
			{
				int last_best_fit_check = total_memory + 1; //Arbitrary value to begin the check on slots
				int temp; // declaration of a temporary variable to hold current slot size while iterating
				bool slot_found = false; //boolean value to confirm a slot was found 
				int slot = 0; //variable to hold the byte size of the slot selected by best fit strategy
				Node *rq_head_ref = memory; // node reference to head to iterate through memory stack
				while(rq_head_ref != NULL)
				{
					/* check for available free space with enough memory to hold new process */
					if((rq_head_ref->data.name == "Free") && (rq_head_ref->data.memory_size >= byte_num))
					{
						/* confirming a slot was found and comparing between previous slot to find best fit slot */
						slot_found = true;
						temp = rq_head_ref->data.memory_size;
						if(temp < last_best_fit_check)
						{
							last_best_fit_check = temp;
						}
					}
					rq_head_ref = rq_head_ref->next; /* move to next memory slot */
				}
				slot = last_best_fit_check; /* assigning the slot size of the best fit slot located */
				/* if no slot was found, there are no slots with enough memory to hold process
				 * and compaction must be used since it was confirmed earlier there is enough total memory available
				 * to hold new process
				 */
				if(slot_found != true)
				{
					std::cout << "Memory is available! No slot large enough for your request."
					<< " Please use command 'C' for compaction" << std::endl;
				}
				/* iterate through the memory stack to locate the 
				 * slot already found previously and insert the new process 
				 */
				else 
				{
					Node *memory_ref = memory; /* Node reference to head of memory stack to iterate through */
					/* variable to make sure the first best fit slot is used in the event 2 or more 
					 * open slots have the same size of bytes
					 */
					int slot_used = 0;  
					/* iteration to find slot and add the new process */
					while(memory_ref != NULL)
					{
						if((memory_ref->data.name == "Free") && (memory_ref->data.memory_size == slot) && slot_used < 1)
						{
							Node *prev_ref = memory_ref->prev; /* reference to previous slot to the best fit slot */
							/* initializing the new process object with the process data */
							process new_request;
							new_request.name = p_name;
							new_request.start_byte = memory_ref->data.start_byte;
							new_request.end_byte = (new_request.start_byte + byte_num) - 1;
							new_request.memory_size = byte_num;
							new_request.p_status = "Active";
							new_request.attribute = "Process";
							memory_ref->data.start_byte = new_request.end_byte + 1;
							memory_ref->data.memory_size -= byte_num;
							if(prev_ref != NULL) /* if best fit slot is between two other slots */
							{
								insertAfter(prev_ref, new_request);
							}
							/* else then best fit slot is the first slot in the memory with no previous slot*/
							else head_insert(&memory, new_request); 
							available_memory -= byte_num; /* updating available memory */
							slot_used++; 
						}
						memory_ref = memory_ref->next;
					}
				}
			}
		}
		/* if user enters a release process command 
		 * call release function and return the amount of memory released
		 */
		else if(((user_input[0] == 'R') || (user_input[0] == 'r')) && ((user_input[1] == 'L') || (user_input[1] == 'l')))
		{
			istringstream buffer(user_input);
			buffer >> cmand >> p_name;
			std::string release_p_name = p_name;
			int amount_released = released_func(memory, release_p_name);
			available_memory += amount_released;
		}
		/* if user enters the compaction command */
		else if(((user_input[0] == 'C') || (user_input[0] == 'c')))
		{
			Node *head_ref1 = memory; /* Node reference to head of memory stack to iterate through */
			process compacted_space; /* object to hold the combined data of the free spaces available */
			compacted_space.name = "Free";
			compacted_space.p_status = "Released";
			compacted_space.end_byte = total_memory - 1;
			int compact_space_bytes = 0; /* variable to hold the total size of free space available */
			while(head_ref1 != NULL)
			{
				if((head_ref1->data.name == "Free"))
				{
					compact_space_bytes += head_ref1->data.memory_size;
				}
				head_ref1 = head_ref1->next;
			}
			compacted_space.memory_size = compact_space_bytes;
			compacted_space.start_byte = (total_memory - 1) - compact_space_bytes;
			compacted_space.attribute = "End";
			Node *compacted_list = NULL; /* declaration of the new list to add in the processes and the compacted slot */
			Node *head_ref2 = memory; /* new reference to head to memory to iterate again to get the processes */
			int next_start_byte = 0;
			while(head_ref2 != NULL)
			{
				/* finding all the processes and adding to the new memory stack pointer */
				if((head_ref2->data.name != "Free"))
				{
					process current_process = head_ref2->data;
					current_process.start_byte = next_start_byte;
					current_process.end_byte = (current_process.start_byte + current_process.memory_size) - 1;
					next_start_byte = current_process.end_byte + 1;
					endQueue_insert(&compacted_list, current_process);
				}
				head_ref2 = head_ref2->next;
			}
			/* adding the compacted free memory to the memory stack new pointer after the processes are added */
			endQueue_insert(&compacted_list, compacted_space);
			Node *old_memory_list = memory; /* arbituray pointer to old memory stack */
			memory = compacted_list; /* reassigning the original pointer to the new stack */
			std::cout << "Success: Memory compaction completed!" << std::endl;
		}
		/* if user enters 'stat' command call stat function to print memory status */
		else if(user_input == "STAT" || user_input == "stat")
		{
			stat_func(memory, total_memory);
		}
		/* if the user entered an unsupported command */
		else 
		{
			std::cout << "UNSUPPORTED COMMAND!" << std::endl;
		}
		/* Bring up menu option again, to handle next command until user quits */
		std::cout << std::endl;
		//std::cout << "Available Memory: " << available_memory << " Bytes" << std::endl;
		std::cout << "Enter: ('RQ', 'RL', 'C', 'STAT', 'QUIT')" << std::endl;
		std::cout << "allocator>";
		std::getline(std::cin, user_input);
	}
}