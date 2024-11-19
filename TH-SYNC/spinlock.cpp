#include <iostream>
#include <pthread.h>
#include <chrono>

pthread_spinlock_t spinlock;

class Integer {
private:
  int inner_value = 0;
public:
  Integer(int value) :inner_value(value) {}
  void inc() {
	pthread_spin_lock(&spinlock);
	inner_value++;
	pthread_spin_unlock(&spinlock);
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
  pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
  auto start_t = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < thread_count; ++i) {
    pthread_create(&tids[i], NULL, increment, num);  
  }

  for (int i = 0; i < thread_count; ++i) {
    pthread_join(tids[i], NULL);
  }
  pthread_spin_destroy(&spinlock);
  auto stop_t = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_t - start_t);
  std::cout << "Duration is " << duration.count() << std::endl;
  std::cout << "Final value is " << num->get_inner_value() << std::endl;
  
  delete[] tids;
  delete num;
  return 0;
}
