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

	char *str_ptr;
	char message[100];
	//char visit[] = "침입감지 ";
	int str_len=0;
	int state;	
	pid_t pid;
	struct sigaction act;
	int pin = 17;
	int value;

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
			gpioInitialise();
			gpioSetMode(pin,PI_INPUT);
			while(1)
			{
				value = gpioRead(pin);	
				printf("%d",value);	
				if(value == 1)
				{
					printf("hello\n");
					//str_ptr=inet_ntoa(serv_addr.sin_addr);
					//strcpy(message,str_ptr);
					//strcat(message,visit);
					char message[30] = "침입감지";
					write(clnt_sock,message,100);
				}	
				gpioDelay(500000);
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
