#include <stdio.h>
#include <pigpio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

void read_childproc(int sig);
void error_handling(char *message);
// 라즈베리파이를 서버로두고
// 클라에서 보내주는 수만큼 더해주거나 빼기로
int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;
	int count = 0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	char message[30];
	int str_len=0;
	int state;	
	pid_t pid;
	struct sigaction act;
	

	if(argc!=2){
		printf("usage: %sport\n",argv[0]);
		exit(1);
	}

	act.sa_handler=read_childproc;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	state=sigaction(SIGCHLD,&act,0);


	serv_sock = socket(PF_INET,SOCK_STREAM,0);
	if(serv_sock == -1)
	{
		error_handling("socket error");
	}

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
	{
		error_handling("bind error");
	}
	if(listen(serv_sock,5)==-1)
	{
		error_handling("listen error");
	}


	while(1)
	{
		clnt_addr_size=sizeof(clnt_addr);
		clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
		if(clnt_sock==-1)
		{
			continue;
		}else
			puts("new client connected");

		pid=fork();
		if(pid==0)
		{
			close(serv_sock);
			while((str_len=read(clnt_sock,message,30))!=0)
			{	
	
		
			int get = atoi(message);
			count+=get;
		
			printf("현재값 = %d",count);
			gpioInitialise();

			gpioSetMode(18, PI_OUTPUT);
			gpioSetMode(27, PI_OUTPUT);
			gpioSetMode(22, PI_OUTPUT);
			gpioSetMode(6, PI_OUTPUT);
			gpioSetMode(25, PI_INPUT);
			gpioSetMode(12, PI_INPUT);
		
			gpioWrite(18,0);
			gpioWrite(27,0);
			gpioWrite(22,0);
			gpioWrite(6,0);
			int current = 0;
			current = count%16;
			switch(current){
				case 0:
					gpioWrite(18,0);
					gpioWrite(27,0);
					gpioWrite(22,0);
					gpioWrite(6,0);
					break;
				case 1:
					gpioWrite(18,1);
					gpioWrite(27,0);
					gpioWrite(22,0);
					gpioWrite(6,0);
					break;
				case 2:
					gpioWrite(18,0);
					gpioWrite(27,1);
					gpioWrite(22,0);
					gpioWrite(6,0);
					break;
				case 3:
					gpioWrite(18,1);
					gpioWrite(27,1);
					gpioWrite(22,0);
					gpioWrite(6,0);
					break;
				case 4:
					gpioWrite(18,0);
					gpioWrite(27,0);
					gpioWrite(22,1);
					gpioWrite(6,0);
					break;
				case 5:
					gpioWrite(18,1);
					gpioWrite(27,0);
					gpioWrite(22,1);
					gpioWrite(6,0);			
					break;
				case 6:
					gpioWrite(18,0);
					gpioWrite(27,1);
					gpioWrite(22,1);
					gpioWrite(6,0);
					break;
				case 7:
					gpioWrite(18,1);
					gpioWrite(27,1);
					gpioWrite(22,1);
					gpioWrite(6,0);
					break;
				case 8:
					gpioWrite(18,0);
					gpioWrite(27,0);
					gpioWrite(22,0);
					gpioWrite(6,1);
					break;
				case 9:
					gpioWrite(18,1);
					gpioWrite(27,0);
					gpioWrite(22,0);
					gpioWrite(6,1);
					break;
				case 10:
					gpioWrite(18,0);
					gpioWrite(27,1);
					gpioWrite(22,0);
					gpioWrite(6,1);
					break;
				case 11:
					gpioWrite(18,1);
					gpioWrite(27,1);
					gpioWrite(22,0);
					gpioWrite(6,1);
					break;
				case 12:
					gpioWrite(18,0);
					gpioWrite(27,0);
					gpioWrite(22,1);
					gpioWrite(6,1);
					break;
				case 13:
					gpioWrite(18,1);
					gpioWrite(27,0);
					gpioWrite(22,1);
					gpioWrite(6,1);
					break;
				case 14:
					gpioWrite(18,0);
					gpioWrite(27,1);
					gpioWrite(22,1);
					gpioWrite(6,1);
					break;
				case 15:
					gpioWrite(18,1);
					gpioWrite(27,1);
					gpioWrite(22,1);
					gpioWrite(6,1);
					break;
				}
				write(clnt_sock,message,str_len);
			}
			close(clnt_sock);
			return 0;
		}
		else
			close(clnt_sock);
	}
	close(serv_sock);
	gpioTerminate();
	return 0;
}
void error_handling(char *message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
void read_childproc(int sig)
{
	pid_t pid;
	int status;
	pid=waitpid(-1,&status,WNOHANG);
}
