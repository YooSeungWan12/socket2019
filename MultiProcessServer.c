#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

#define PORT 10000
#define BUFSIZE 10000
char buffer[BUFSIZE] = "Hi, I'm Server\n";
//sizeof(buffer)=>100(배열의크기)
//strlen(buffer)=>15(buffer의크기)
char rcvBuffer[BUFSIZE];
char hiBuffer[100]= "안녕하세요.만나서 반가워요.";
char nameBuffer[100]= "내 이름은 유승완이야.";
char ageBuffer[100]= "나는 25살이야.";
int numClient = 0;

void do_service(int c_socket);
void sig_handler();

int main(){
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int n;
	int pid;
	signal(SIGCHLD, sig_handler);//첫번째 인자 : 시그널번호,두번째 인자 : 첫번째 인자의 시그널이 발생했을 때 실행되는 함수명
	// 1. 서버 소켓 생성
	//서버 소켓 = 클라이언트의 접속 요청을 처리(허용)해 주기 위한 소켓
	s_socket = socket(PF_INET, SOCK_STREAM, 0); //TCP/IP 통신을 위한 서버 소켓 생성
	
	//2. 서버 소켓 주소 설정
	memset(&s_addr, 0, sizeof(s_addr)); //s_addr의 값을 모두 0으로  초기화
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP 주소 설정
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);

	//3. 서버 소켓바인딩
	if(bind(s_socket,(struct sockaddr *) &s_addr, sizeof(s_addr)) == -1){ 
		//바인딩 작업 실패 시, Cannot Bind 메시지 출력 후 프로그램 종료
		printf("Cannot Bind\n");
		return -1;
	}
	
	//4.listen() 함수 실행
	if(listen(s_socket, 5) == -1){
		printf("listen Fail\n");
		return -1;
	}

	//5. 클라이언트 요청 처리
	// 요청을 허용한 후, Hello World 메세지를 전송함
	while(1){ //무한 루프
		len = sizeof(c_addr);
		printf("클라이언트 접속을 기다리는 중....\n");
		c_socket = accept(s_socket, (struct sockaddr *)&c_addr, &len); 
		printf("/client is connected\n");
		numClient++;
		printf("현재 접속 중인 클라이언트 수: %d\n",numClient);
		//클라이언트의 요청이 오면 허용(accept)해 주고, 해당 클라이언트와 통신할 수 있도록 클라이언트 소켓(c_socket)을 반환함.
		if((pid = fork()) > 0){//부모 프로세스
		close(c_socket);
		continue;
		}else if (pid == 0){//자식 프로세스
		close(s_socket);
		do_service(c_socket);
		exit(0);
		}else{
		printf("실패\n");
		exit(0);
}
	}
	close(s_socket);
	return 0;	
}


void do_service(int c_socket){
		int n;
		
		while(1){
					n=read(c_socket,rcvBuffer,sizeof(rcvBuffer));//추가내용
					printf("rcvBuffer: %s\n",rcvBuffer);
					rcvBuffer[n-1] = '\0';//개행문자 삭제
					if(strncasecmp(rcvBuffer,"quit",4) == 0 || strncasecmp(rcvBuffer,"kill server",11) == 0)
						break;
					else if(!strncasecmp(rcvBuffer,"안녕하세요.",strlen("안녕하세요")))
						strcpy(buffer,"안녕하세요. 만나서 반가워요.");
					else if(!strncasecmp(rcvBuffer,"이름이 머야?",strlen("이름이 머야?")))
						strcpy(buffer,"내 이름은 유승완이야.");
					else if(!strncasecmp(rcvBuffer,"몇 살이야?",strlen("몇 살이야?")))
						strcpy(buffer,"나는 25살이야.");
					else if(!strncasecmp(rcvBuffer,"strlen ",strlen("strlen ")))
						sprintf(buffer,"문자열의 길이는 %d입니다.",strlen(rcvBuffer)-7);//문자열의 길이는 XX입니다.
					else if(!strncasecmp(rcvBuffer,"strcmp ",strlen("strcmp "))){
						char *token;
						char *str[3];
						int idx = 0;

						token = strtok(rcvBuffer," ");
						while(token != NULL){
							str[idx] = token;
							printf("str[%d] = %s\n",idx,str[idx]);
							idx++;
							token = strtok(NULL," ");

						}
						if(idx < 3)
							strcpy(buffer, "문자열 비교를 위해서는 두 문자열이 필요합니다.");
						else if(!strcmp(str[1],str[2]))//같은 문자열이면
							sprintf(buffer, "%s와 %s는 같은 문자열입니다.",str[1],str[2]);	
						else
							sprintf(buffer, "%s와 %s는 다른 문자열입니다.",str[1],str[2]);	

					}
					else if(!strncasecmp(rcvBuffer,"readfile ",strlen("readfile "))){
					FILE *fp;
					int cnt=0;
					char *str[10];
					char *token;
					char *command;
					token = strtok(rcvBuffer," ");// token = readfile
					token = strtok(NULL," "); // token=<파일명>
					//command= strtok(NULL,"\0");
					//while(token != NULL){
						//str[cnt] = token;//str[0]=readfile,str[1]=파일명
						//cnt++;
					//	token = strtok(NULL, " ");//token=파일명
					//}
					//if(cnt < 2){
						//strcpy(buffer,"파일명을 입력해주세요");
					//}else{
					
							fp = fopen(token,"r");//읽기모드로 열겠다.
			
			
							if(fp){//정상적으로 파일이 오픈되었다면.
								//char	tempStr[BUFSIZE];  파일 내용을 저장할 변수.
								memset(buffer,0,BUFSIZE);//buffer초기화
			
								while(fgets(rcvBuffer,BUFSIZE,(FILE *)fp)){//while(fgets(tempStr, 2500, (FILE *)fp))
									strcat(buffer,rcvBuffer);//여러줄의 내용을 하나의 buffer에 저장하기위해 strcat()함수 사용				
								}
							fclose(fp);
							}else{//해당 파일이 없는경우,
								strcpy(buffer,"해당 파일은 존재하지 않습니다.");

							}

						}
					//}
				else if(!strncasecmp(rcvBuffer,"exec",strlen("exec"))){
		//		else if(!strncasecmp(rcvBuffer,"exec ", 5)){
		
					char *token;
					char *command;
					token = strtok(rcvBuffer," ");//token = exec
					command= strtok(NULL,"\0");//exec뒤에있는 모든 문자열을 command로저장
					printf("command:%s\n",command);
		//			int result = system(command);
			//		if(!result)
				//{
				//	sprintf(buffer,"[%s] command is executed",command);
				//}
				//	else{
					//sprintf(buffer,"[%s] command is failed",command);
				//}
		
					if(system(command)==0)
					sprintf(buffer,"[%s] command is executed",command);
					else
					sprintf(buffer,"[%s] command is failed",command);
				}
					else
						strcpy(buffer,"무슨 말인지 모르겠습니다.");

				write(c_socket,buffer,strlen(buffer));
				}

				close(c_socket);

}


void sig_handler(int signo){
	int pid;
	int status;
	pid = wait(&status);
 	printf("pid[%d] is terminated.status = %d\n",pid,status);
	numClient--;
	printf("현재 접속 중인 클라이언트 수 : %d\n",numClient);

}
