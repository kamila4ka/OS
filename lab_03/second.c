#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void getLocalTime(int* mas){
	time_t timer = time(NULL);
	struct tm localTime = *localtime(&timer);
				
	mas[0] = localTime.tm_hour;
	mas[1] = localTime.tm_min;
	mas[2] = localTime.tm_sec;
}

int main(){
	
	key_t key = ftok("tmpfile", 1);
	
	int shmid = shmget(key, 128, IPC_EXCL | 0666);
        if(shmid == -1){
		printf("you probably haven't written anything in the file\n");
		exit(-1);
	}	

	char* shmaddr = shmat(shmid, NULL, 0);
	if(shmaddr == (char*) (- 1)){
		perror("can't attach shared memory");
		exit(-1);
	}

	while(1){
		sleep(2);
		int mas[3];
		getLocalTime(mas);
		printf("Time: %d:%d:%d. Pid of receiving process: %d\nReceived message: %s\n", 
			mas[0], mas[1], mas[2], getpid(), shmaddr);
	}

	return 0;

}
