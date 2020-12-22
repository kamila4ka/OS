#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int numOfThreads = 11;
int counter = 0;
int* buf;
int sizeOfBuf = 100;

pthread_mutex_t mutex;
pthread_cond_t cond;

void* threadWrite(){
	while(1){
		pthread_mutex_lock(&mutex);
		++counter;
		if(counter < sizeOfBuf) buf[counter] = counter;
		printf("СЧЕТЧИК == %d\n", counter);
		fflush(stdout);
		
		int randomNumber = rand() % 2;
		if(randomNumber){
			pthread_cond_broadcast(&cond);
		}
		else{
			pthread_cond_signal(&cond);
		}
		pthread_mutex_unlock(&mutex);
		sleep(2);
	}
}

void* threadRead(){
	while(1){
		pthread_mutex_lock(&mutex); 
		pthread_cond_wait(&cond, &mutex);
		printf("Идентификатор потока: %ld. Размер массива: %d\n", (long)pthread_self(), 
				buf[counter]);
		fflush(stdout);
		pthread_mutex_unlock(&mutex);
		sleep(rand() % 10);
	}
}

int main(){
	buf = (int*)calloc(sizeOfBuf, sizeof(int));
	
	int i;
	pthread_t threads[numOfThreads];

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	for(i = 0; i < 1; i++){
		pthread_create(&threads[i], NULL, threadWrite, NULL);
	}
	for( ; i < numOfThreads; i++){
		pthread_create(&threads[i], NULL, threadRead, NULL);
	}

	for(i = 0; i < numOfThreads; i++){
		pthread_join(threads[i], NULL);
	}

	pthread_mutex_destroy(&mutex);

	free(buf);
	return 0;
}
