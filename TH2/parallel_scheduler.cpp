#include "parallel_scheduler.h"

parallel_scheduler::parallel_scheduler(size_t capacity) : capacity(capacity), stop(false){
	threads = new pthread_t[capacity];
	pthread_mutex_init(&mutex, nullptr);
	pthread_cond_init(&cond, nullptr);

	for(int i = 0; i < capacity; i++){
		pthread_create(&threads[i], nullptr, thread_worker, this);
	}
}

parallel_scheduler::~parallel_scheduler(){
	pthread_mutex_lock(&mutex);
	stop = true;
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mutex);

	for(size_t i = 0; i < capacity; i++){
		pthread_join(threads[i], nullptr);
	}

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	delete[] threads;
}

void parallel_scheduler::run(void (*func)(void*), void* arg){
	pthread_mutex_lock(&mutex);
	task_queue.push({func, arg});
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
}

void* parallel_scheduler::thread_worker(void* arg){
	parallel_scheduler* pool = static_cast<parallel_scheduler*>(arg);

	while(true){
		Task task;
		pthread_mutex_lock(&pool->mutex);

		while(pool->task_queue.empty() && !pool->stop){
			pthread_cond_wait(&pool->cond, &pool->mutex);
		}
		
		if(pool->stop && pool->task_queue.empty()){
			pthread_mutex_unlock(&pool->mutex);
			break;
		}

		task = pool->task_queue.front();
		pool->task_queue.pop();

		pthread_mutex_unlock(&pool->mutex);

		task.func(task.arg);
	}

	return nullptr;
}
