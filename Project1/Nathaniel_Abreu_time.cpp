/* Author: Nathaniel Abreu
 * CS340 Project 1
 * Professor: John Svadlenka
 * To Run file, compile first with "g++ time.cpp"
 * Then enter "./a.out"
 */

#include <sys/types.h> 
#include <sys/time.h>
#include <stdio.h> 
#include <string.h> 
#include <unistd.h>
#define READ_END 0 
#define WRITE_END 1
#include <string>
#include <iostream>
#include <stddef.h>
#include <sys/wait.h>
#define BUFFER_SIZE 25
#include <stdlib.h>

int main() 
{ 
	/* variable declarations for pipes, char array, process and user input */
	int fd[2];
	int fd2[2];
	char read_sec[BUFFER_SIZE];
	char read_msec[BUFFER_SIZE];
	pid_t pid;
	std::string enter_cmd;

	/* Ask user to enter a command and assign it to string declaration*/
	std::cout << "Enter one of the following commands: ls, ps," << std::endl;
	std::cout << "whoami, hostname, date" << std::endl;
	std::cin >> enter_cmd;
	
	/* create the pipe */ 
	if ((pipe(fd) == -1) || (pipe(fd2) == -1)) 
	{ 
		fprintf(stderr,"Pipe failed"); 
		return 1; 
	} 
	
	/* fork a child process */ 
	pid = fork();

	if (pid < 0) /* error occurred */ 
	{ 
		fprintf(stderr, "Fork Failed"); 
		return 1; 
	} 
	else if (pid == 0) /* child process */
	{	 
		/* close the unused end of the pipe */ 
		close(fd[READ_END]);

		/* Get current sec and micro sec and convert to char array */
		struct timeval current;
        	gettimeofday(&current, NULL);
        	int child_sec = current.tv_sec;
		int child_msec = current.tv_usec;
		std::string child_secString = std::to_string(child_sec);
		std::string child_msecString = std::to_string(child_msec);
		int sec_len = child_secString.length();
		int msec_len = child_msecString.length();
		char time_startsec[sec_len + 1];
		char time_startmsec[msec_len + 1];
		strcpy(time_startsec, child_secString.c_str());
		strcpy(time_startmsec, child_msecString.c_str());
		
		/* Write to pipes 1 and 2 */
		write(fd[WRITE_END], time_startmsec, strlen(time_startmsec)+1);
		write(fd2[WRITE_END], time_startsec, strlen(time_startsec)+1);

		std::cout << "Child: Start Time = " << child_sec << "."
		<< child_msec << std::endl;

		std::cout << "Name of command: " << enter_cmd << std::endl;
		std::cout << "Output of Command" << std::endl;
		/* Command functions */
		if(enter_cmd == "ls")	
		{	
			execlp("ls","ls",NULL);
		}
		else if(enter_cmd == "ps")
		{
			execlp("ps","ps",NULL);
		}
		else if(enter_cmd == "whoami")
		{
			execlp("whoami", "whoami", NULL);
		}
		else if(enter_cmd == "hostname")
		{
   			execlp("hostname", "hostname", NULL);
		}
		else if(enter_cmd == "date")
		{
			execlp("date", "date", NULL);
		}
		else
		{
			std::cout << "NONE! Unsupported Command!" << std::endl;
			return 0;
		}
	} 	
	else /* parent process */ 
	{
		/* Wait for child process to complete */
		wait(NULL);

		/* Mark finish time */
		struct timeval current;
        	gettimeofday(&current, NULL);
		int parent_sec = current.tv_sec;
        	int parent_msec = current.tv_usec;
		
		/* close the unused end of the pipe */ 
		close(fd[WRITE_END]);

		/* Read from pipe */
		read(fd2[READ_END], read_sec, BUFFER_SIZE);
		read(fd[READ_END], read_msec, BUFFER_SIZE);

		/* Parsing char array back to int type */
		int parsed_sec = 0;
		int parsed_msec = 0;
		for(int i = 0; i < strlen(read_sec); i++)
		{
			parsed_sec = (parsed_sec * 10) + (read_sec[i] - '0');
		}
		for(int i2 = 0; i2 < strlen(read_msec); i2++)
		{
			parsed_msec = (parsed_msec * 10) + (read_msec[i2] - '0');
		}
		int total_sec = parent_sec - parsed_sec;
		int total_msec = parent_msec - parsed_msec;

		/* Output parent times and the elapse time from child time */
		std::cout << "Parent: Start Time = " << read_sec << "."
		<< read_msec << std::endl;

		std::cout << "Parent: Ending Time = " << parent_sec << "."
		<< parent_msec << std::endl;
		
		std::cout << "Parent: Elapsed Time = " << total_sec << "."
		<< total_msec << std::endl;	
	} 
	return 0;
}