#include <iostream>
#include <unistd.h>
#include "parallel_scheduler.h"

void example_task(void* arg) {
    int id = *reinterpret_cast<int*>(arg);
    std::cout << "Task " << id << " is being executed by thread "
              << pthread_self() << std::endl;
    sleep(1);
}

int main() {
    const size_t thread_pool_size = 4;
    parallel_scheduler scheduler(thread_pool_size);

    const int num_tasks = 10;
    int task_args[num_tasks];
    for (int i = 0; i < num_tasks; ++i) {
        task_args[i] = i;
        scheduler.run(example_task, &task_args[i]);
    }
    return 0;
}
