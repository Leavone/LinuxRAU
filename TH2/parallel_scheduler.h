#include <pthread.h>
#include <queue>
#include <iostream>

class parallel_scheduler{
private:
	struct Task{
		void (*func)(void*);
		void* arg;
	};
	size_t capacity;

	std::queue<Task> task_queue;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	pthread_t* threads;
	bool stop;
	
	static void* thread_worker(void* arg);

public:
	parallel_scheduler(size_t capacity);
	~parallel_scheduler();

	void run(void (*func)(void*), void* arg);

};
