#include <pthread.h>
#include <iostream>

pthread_mutex_t mutex;

class BankAccount{
private:
	int balance;
public:
	BankAccount(int balance) : balance(balance){}
	void debit(int amount){
		balance -= amount;
	}
	void credit(int amount){
		balance += amount;
	}
	int get_balance(){
		return balance;
	}

};

struct thread_info{
	bool is_debiter;
	BankAccount* bank_account;
};

void* thread_func(void* arg){
	struct thread_info* info = (thread_info *)arg;
	for(int i = 0; i < 10000; i++){
		if(info->is_debiter){
			pthread_mutex_lock(&mutex);
			info->bank_account->debit(100);
			pthread_mutex_unlock(&mutex);
		} else {
			pthread_mutex_lock(&mutex);
       	        	info->bank_account->credit(50);
	                pthread_mutex_unlock(&mutex);
		}
	}
	return nullptr;
}

int main(){
	if(pthread_mutex_init(&mutex, NULL) != 0){
		std::cerr<<"error creating mutex";
		return 1;
	}

	BankAccount bank_account(200);
	thread_info infos[2];
	infos[0].is_debiter=true;
	infos[0].bank_account = &bank_account;
	infos[1].is_debiter=false;
        infos[1].bank_account = &bank_account;
	pthread_t tids[2];
	if(
		pthread_create(&tids[0], NULL, thread_func, (void *)&infos[0]) != 0 ||
		pthread_create(&tids[1], NULL, thread_func, (void *)&infos[1]) != 0)
	{
		std::cerr<<"error creating threads";
		return 1;
	}
	if(
		pthread_join(tids[0], NULL) != 0 ||
		pthread_join(tids[1], NULL) != 0)
	{
		std::cerr<<"error joining thread";
		return 1;
	}
	std::cout<<bank_account.get_balance()<<'\n';
	if(pthread_mutex_destroy(&mutex) != 0){
		std::cerr<<"error destroying mutex";
		return 1;
	}
}
