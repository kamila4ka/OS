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
#include <sys/sem.h>

struct sembuf semwait = { 0, -1, SEM_UNDO };
struct sembuf semsignal = { 0, 1, SEM_UNDO };

void getLocalTime(int* mas){
	time_t timer = time(NULL);
	struct tm localTime = *localtime(&timer);
				
	mas[0] = localTime.tm_hour;
	mas[1] = localTime.tm_min;
	mas[2] = localTime.tm_sec;
}

int main(int argc, char* argv[]){

	key_t key = ftok("file", 1);
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
			perror("can't get memory");
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
		exit(-1);
	}

	key_t semkey = ftok("sem", 1);
	int semid = semget(semkey, 1, 0666);

	if(semid == -1){
		printf("can't get semaphores id\n");
		exit(-1);
	}
	

	while(1){
		
		int semsignalRes = semop(semid, &semsignal, 1);

		if(semsignalRes == -1){
			printf("error unlocking semaphores\n");
			exit(-1);
		}

		int mas[3];
		getLocalTime(mas);
		sprintf(shmaddr, "Time: %d:%d:%d. Pid of sending process: %d.\n", 
			mas[0], mas[1], mas[2], getpid());
	

		int semwaitRes = semop(semid, &semwait, 1);
		if(semwaitRes == -1){
			printf("error locking semaphores\n");
			exit(-1);
		}

		sleep(2);
	}

	return 0;

}
