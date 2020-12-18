#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char fifofile[10] = "./tmpfile";

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
	int fifofd;

	mkfifo(fifofile, 0777);
	
	pid_t pid = fork();


	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	//child process
	if(pid == 0){
		fifofd = open(fifofile, O_RDONLY);
		if(fifofd){
			sleep(5);
			read(fifofd, buf, bufsize);		
			int* childmas = (int*)calloc(3, sizeof(int));
			getLocalTime(childmas);
			printf("Local time of the child process: %d:%d:%d.\n\nReceived message: %s.\n", 
				childmas[0], childmas[1], childmas[2], buf);
			close(fifofd);
			free(childmas);
		}	
	}

	//parent process
	else{
		fifofd = open(fifofile, O_WRONLY);
		if(fifofd){
		        int* mas = (int*)calloc(3, sizeof(int));
		        getLocalTime(mas);
		        sprintf(buf, "Message from parent process.\nPid: %d. Time: %d:%d:%d\n",
					getpid(), mas[0], mas[1], mas[2]);
		        write(fifofd, buf, bufsize);

		        close(fifofd);
		        free(mas);		
		}	
	}

	return 0;
}
