#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

void getLocalTime(int* mas){
	time_t timer = time(NULL);
	struct tm localTime = *localtime(&timer);
	
	mas[0] = localTime.tm_hour;
	mas[1] = localTime.tm_min;
	mas[2] = localTime.tm_sec;

	return;
}

int main(){
	char buf[80];
	const int bufsize = 80;
	char localTimeString[60];

	int pipefd[2];
	pipe(pipefd);
	pid_t pid = fork();


	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}
	//child process
	if(pid == 0){
		sleep(5);
		//child doesn't need write end
		close(pipefd[1]);
		if(read(pipefd[0], buf, bufsize)){		
			int* childmas = (int*)calloc(3, sizeof(int));
			getLocalTime(childmas);
			fflush(stdout);
			printf("Local time of the child process: %d:%d:%d.\n\nReceived message: %s.\n", 
				childmas[0], childmas[1], childmas[2], buf);
			free(childmas);
		}
		close(pipefd[0]);	
	}
	//parent process
	else{
		//parent doesn't need read end
		close(pipefd[0]);
		int* mas = (int*)calloc(3, sizeof(int));
		getLocalTime(mas);
		sprintf(buf, "Message from parent process.\nPid: %d. Time: %d:%d:%d\n",
				getpid(), mas[0], mas[1], mas[2]);
	        write(pipefd[1], buf, bufsize);	

		close(pipefd[1]);
		free(mas);	
	}

	return 0;
}
