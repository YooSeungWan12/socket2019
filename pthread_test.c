#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> //쓰레드생성시 필수

void *func_thread(void *); //스레드가 수행할 함수
//void * :최상위형태,Java의 Object와비슷한개념.모든타입으로 형변환가능

int main(){
	int status;
	pthread_t thread_id;
	pthread_create(&thread_id,NULL,func_thread,NULL);//func_thread는 실행할 함수
	
	printf("thread %x is creatd\n",thread_id);
	pthread_join(thread_id, (void **)&status);
	printf("main thread END\n");
	return 0;


}

void *func_thread(void *argv){
	int i=0;

	while(i<10){
		i++;
		printf("thread %dth executing.....\n",i);
	}
}
