#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

int t,n;

typedef struct prime_search{
	int num;
	pthread_mutex_t lock;
	pthread_cond_t cond;
	int count;
	int conditionMet;
	
}prime_search_t;

prime_search_t p_search; 

int 
is_prime(int n){
	int c;
	if(n < 2) return 0;
	for(c = 2; c < n/2; c++){
		if((n%c) == 0)
			return 0;
	}
	return 1;
}
static void *
search_threads(void *arg){

	int *id = (int *)arg;
	
	while(!p_search.conditionMet){
		sleep(1);
		pthread_mutex_lock(&p_search.lock);	
			p_search.num++;
			if(is_prime(p_search.num)){
				printf("%d\n",p_search.num);
				printf("thread %d searching prime: num: %d \n", *id, p_search.num);
				p_search.count++;
			}			
			while(p_search.count == n){		
				p_search.conditionMet = 1;
				pthread_cond_signal(&p_search.cond);	
				printf("thread %d exiting\n", *id);	
				break;
			}
		pthread_mutex_unlock(&p_search.lock);
	}
	
return NULL;

}
void 
init(){
	p_search.num = 1;
	p_search.count = 0;
	p_search.conditionMet = 0;
	pthread_mutex_init(&p_search.lock, NULL);
	pthread_cond_init(&p_search.cond, NULL);
}

int 
main(int argc, char *argv[]){

	t = atoi(argv[1]);
	n = atoi(argv[2]);
	init();
	
	pthread_t *th_sp;
	th_sp = (pthread_t *)malloc(t * sizeof(pthread_t));
	int i;
	int *pi;
	for(i=0;i<t;i++){
		pi = (int *)malloc(sizeof(int));
		*pi = i;
		sleep(1);
		pthread_create(&th_sp[i],NULL,search_threads, (void *)pi);
	}

	pthread_mutex_lock(&p_search.lock);
	while (!p_search.conditionMet) {
		pthread_cond_wait(&p_search.cond, &p_search.lock);
		break;
	}
	
	pthread_mutex_unlock(&p_search.lock);
	
	for(i=0;i<t;i++){
		pthread_join(th_sp[i],NULL);
	}

	return 0;
}
