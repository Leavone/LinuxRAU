#include <pthread.h>
#include <iostream>
#include <unistd.h>

class Printer{
public:
	void print(std::string to_print){
		std::cout<<"Printing...\n";
		sleep(1);
		std::cout<<to_print<<'\n';
		std::cout<<"Printing Finished\n";
		sleep(1);
	}
};

struct thread_args{
	Printer* printer;
	std::string to_print;
};

bool is_available = true;
pthread_cond_t cond;
pthread_mutex_t mutex;

void* print(void* arguments){
	struct thread_args* args = (struct thread_args*)arguments;
	pthread_mutex_lock(&mutex);
	while(!is_available){
		pthread_cond_wait(&cond, &mutex);
	}
	is_available = false;
	args->printer->print(args->to_print);
	is_available = true;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
	return nullptr;
}

int main(){
        int n;
        std::cin>>n;
	Printer printer;
        thread_args* arguments = new thread_args[n];
	pthread_t* thread_ids = new pthread_t[n];
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
        for(int i = 0; i < n; i++){
                std::string command;
                std::cin>>command;
                arguments[i] = thread_args{&printer, command};
        }

        for(int i = 0; i < n; i++){
                int ret = pthread_create(&thread_ids[i], NULL, *print, (void*)&arguments[i]);
                if (ret != 0){
                        std::cerr<<"error creating thread\n";
                        return 1;
                }
        }
        for(int i = 0; i < n; i++){
                int ret = pthread_join(thread_ids[i], NULL);
                if (ret != 0){
                        std::cerr<<"error joining the thread\n";
                        return 1;
                }
        }
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	delete[] arguments;
	delete[] thread_ids;
}
