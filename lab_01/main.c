#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void parentProcessInfo();
void childProcessInfo();

void atexitFunc(){
	pid_t pid = fork();
	if(pid == 0){
		childProcessInfo();
	}
	else{
		parentProcessInfo();
	}
}

void parentProcessInfo(){
	pid_t pid = getpid();
	int status;
	printf("Attribute: Parent. PID: %d.\n", pid);
	wait(&status);
	printf("Exiting parent process\n");
	exit(0);
}

void childProcessInfo(){
	pid_t pid = getpid();
	printf("Attribute: Child. PID: %d. Parent PID: %d.\n", pid, getppid());
	printf("Exiting child process\n");
	exit(0);
}

int main(){
	if(atexit(atexitFunc)){
		printf("Error: unregistered function.\n");		
	}

	return 0;
}
