#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define BUF_SIZE 2056


void error_handling(char *message);

//  소켓을 생성해서 connect하고 recv해서 받아온걸 버퍼에 저장해서 보여주기

int main(int argc,char *argv[])
{
	int sock;
	char buf[BUF_SIZE];
	struct sockaddr_in serv_addr; // 연결할 서버의 주소
	int byteCount;
        struct hostent *host;

	if(argc!=3)
	{
		printf("need domain & port\n");
		exit(1);
	}
	sock = socket(PF_INET,SOCK_STREAM,0);
	if(sock == -1)
	{
		error_handling("socket() error");
	}

	host = gethostbyname(argv[1]);

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr = *(struct in_addr*)host->h_addr_list[0];
	serv_addr.sin_port = htons(atoi(argv[2]));

	if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
	{
		error_handling("connect() error\n");
	}

	// send header
	char *header = "Get /webhp HTTP/1.1\r\nUser-Agent: Mozilla/4.0\r\ncontent-type:text/html\r\nConnection: close\r\n\r\n";
	send(sock,header,strlen(header),0);
	
	byteCount = recv(sock,buf,sizeof(buf),0);
	buf[byteCount] = 0; // 문자열 종료
	printf("%s",buf);
	return 0;
}


void error_handling(char *message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
