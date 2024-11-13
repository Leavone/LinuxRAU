#include <iostream>
#include <cstdlib>
#include <chrono>
#include <pthread.h>



struct Args{
	int* arr;
	int start;
	int end;
	int id;
	int* results;
};

void* partial_sum(void* args){
	Args* arguments = (Args*) args;
	arguments->results[arguments->id] = 0;
	for(int i = arguments->start; i < arguments->end; i++){
		arguments->results[arguments->id] += arguments->arr[i];
	}
	return nullptr;
}

int main(int argc, char* argv[]){
	if(argc != 3){
		std::cerr<<"Not enough arguments\n";
		return 1;	
	}
	int arr_size = atoi(argv[1]);
	int num_threads = atoi(argv[2]);
	int* array = new int[arr_size];
	std::srand(static_cast<unsigned>(std::time(0)));
	for(int i = 0; i < arr_size; i++){
		array[i] = std::rand() % 100;
	}
	int sum = 0;
	auto start = std::chrono::high_resolution_clock::now();
	for(int i = 0; i < arr_size; i++){
		sum += array[i];
	}
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	int* results = new int[num_threads];
	pthread_t* tids = new pthread_t[num_threads];

	Args* arguments_list = new Args[num_threads];
	auto start_t = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < num_threads - 1; i++){
		int st = (arr_size / num_threads) * i;
                int ed = (arr_size / num_threads) * (i+1);
                arguments_list[i].arr = array;
		arguments_list[i].start = st;
		arguments_list[i].end = ed;
		arguments_list[i].id = i;
		arguments_list[i].results = results;
	}
	int st = (arr_size / num_threads) * (num_threads - 1);
	arguments_list[num_threads-1].arr = array;
        arguments_list[num_threads-1].start = st;
        arguments_list[num_threads-1].end = arr_size;
	arguments_list[num_threads-1].id = num_threads-1;
        arguments_list[num_threads-1].results = results;

	for(int i = 0; i < num_threads; i++){
		pthread_create(&tids[i], NULL, partial_sum, (void *)&arguments_list[i]);	
	}

	for(int i = 0; i < num_threads; i++){
		pthread_join(tids[i], NULL);
	}
	int result = 0;
	for(int i = 0; i < num_threads; i++){
                result += results[i];
        }
	auto stop_t = std::chrono::high_resolution_clock::now();
        auto duration_thread = std::chrono::duration_cast<std::chrono::microseconds>(stop_t - start_t);

	
	std::cout<<"Time spent without threads: " << duration.count()<<'\n';
	std::cout<<"Time spent with threads: " << duration_thread.count()<<'\n';


}
