#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>

int shmid;

void getLocalTime(int* mas){
	time_t timer = time(NULL);
	struct tm localTime = *localtime(&timer);
				
	mas[0] = localTime.tm_hour;
	mas[1] = localTime.tm_min;
	mas[2] = localTime.tm_sec;
}

void deleteSharedMemory(){
	
	if(shmctl(shmid, IPC_RMID, 0) == -1){
		printf("error deleting shared memory\n");
	}
	else{
		printf("successfully deleted shared memory\n");
	}
}


void atexitFunc(){
	struct shmid_ds shminfo;
	shmctl(shmid, SHM_INFO, &shminfo);
	if(shminfo.shm_segsz != 0){
		deleteSharedMemory();
	}
}

int main(int argc, char* argv[]){

	if(atexit(atexitFunc)){
		printf("error in atexit\n");
	}

	key_t key = ftok("tmpfile", 1);
	int shmid = shmget(key, 128, IPC_CREAT | 0666);
	if(argc > 1){
		if(strcmp(argv[1], "-f") == 0){
			 struct shmid_ds *buf = 0;
			 shmctl(shmid, IPC_RMID, buf);
			 exit(0);
		}
	}

	printf("if you'd like to clear memory, run ./first with -f flag\n");	

	if(shmid == -1){
		if(errno == EEXIST){
			printf("file already exists\n");
			exit(-1);
		}

		else{
			perror("can't get memory :(");
			exit(-1);
		}
	}	
	
	char* shmaddr = shmat(shmid, NULL, 0);
	if(shmaddr == (char*)(- 1)){
		perror("can't attach shared memory");
		exit(-1);
	}

	if(strlen(shmaddr) != 0){
		printf("can't run sending program more than once\n");
		exit(0);
	}

	while(1){
		sleep(2);
		int mas[3];
		getLocalTime(mas);
		sprintf(shmaddr, "Time: %d:%d:%d. Pid of sending process: %d.\n", 
			mas[0], mas[1], mas[2], getpid());
	}


	return 0;

}
