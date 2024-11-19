#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <chrono>

sem_t semaphore;

class Integer {
private:
  int inner_value = 0;
public:
  Integer(int value) :inner_value(value) {}
  void inc() {
	sem_wait(&semaphore);
	inner_value++;
	sem_post(&semaphore);
  }
  int get_inner_value() {
    return inner_value;
  }
};

void* increment(void* arg) {
  Integer* num = (Integer*)arg;
  for (int i = 0; i < 10000; ++i) {
    num->inc();
  }
  return nullptr;
}

int main(int argc, char** argv) {
  Integer* num = new Integer(0);
  int thread_count = 50;
  pthread_t* tids = new pthread_t[thread_count];
  sem_init(&semaphore, 0, 1);
  auto start_t = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < thread_count; ++i) {
    pthread_create(&tids[i], NULL, increment, num);  
  }

  for (int i = 0; i < thread_count; ++i) {
    pthread_join(tids[i], NULL);
  }
  sem_destroy(&semaphore);
  auto stop_t = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_t - start_t);
  std::cout << "Duration is " << duration.count() << std::endl;
  std::cout << "Final value is " << num->get_inner_value() << std::endl;
  
  delete[] tids;
  delete num;
  return 0;
}
