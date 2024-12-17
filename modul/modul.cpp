#include <unistd.h>
#include <pthread.h>
#include <iostream>

int i = 1;

bool is_available;
pthread_mutex_t producer;
pthread_mutex_t consumer;
pthread_cond_t cond1;
pthread_cond_t cond2;

struct thread_arguments{
	bool is_producer;
};

void* thread_func(void* arguments){
	struct thread_arguments* args = (struct thread_arguments*)arguments;
	while(true){
		sleep(5);
		if(args->is_producer){
			pthread_mutex_unlock(&consumer);
			pthread_mutex_lock(&producer);
			while(is_available){
				pthread_cond_wait(&cond2, &producer);
			}
			is_available = true;
			i = (i+1)%4;
			pthread_cond_signal(&cond1);
			pthread_mutex_unlock(&producer);
			pthread_mutex_lock(&consumer);
		}
		else{
			pthread_mutex_unlock(&producer);
			pthread_mutex_lock(&consumer);
			while(!is_available){
				pthread_cond_wait(&cond1, &consumer);
			}
			is_available = false;
			std::cout<<"Number " << i+1 <<'\n';
			pthread_cond_signal(&cond2);
	
			pthread_mutex_unlock(&consumer);
			pthread_mutex_lock(&producer);
		}
	}
}

int main(){
	pthread_mutex_init(&producer, NULL);
	pthread_mutex_init(&consumer, NULL);
	pthread_cond_init(&cond1, NULL);
	pthread_cond_init(&cond2, NULL);

	struct thread_arguments prod{true};
	struct thread_arguments cons{false};

	pthread_t tid[2];
	pthread_create(&tid[0], NULL, *thread_func, (void*)&prod);
	pthread_create(&tid[1], NULL, *thread_func, (void*)&cons);

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);


}
