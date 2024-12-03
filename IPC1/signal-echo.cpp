#include <signal.h>
#include <iostream>
#include <pwd.h>
#include <ucontext.h>

void handler(int signum, siginfo_t *info, void *context){
	ucontext_t* ucp = (ucontext_t*)context;
	struct passwd *pws;
	pws = getpwuid(info->si_uid);
	long long rip = ucp->uc_mcontext.gregs[REG_RIP];
	long long rax = ucp->uc_mcontext.gregs[REG_RAX];
	long long rbx = ucp->uc_mcontext.gregs[REG_RBX];
	std::cout<<"Received a SIGUSR1 signal from process " << info->si_pid << " executed by " << info->si_uid << ' ' << pws->pw_name<<'\n';
	std::cout<<"State of the context: RIP = "<<rip<<", RAX = "<<rax<<", RBX = "<<rbx<<".\n";
}

int main(){
	struct sigaction sa;
	sa.sa_sigaction = handler;
	sa.sa_flags = SA_SIGINFO;
	if(sigaction(SIGUSR1, &sa, NULL) != 0){
		std::cerr<<"Couldn't register signal handler\n";
		return 1;
	}
	while(true){
		sleep(10);
	}
}
