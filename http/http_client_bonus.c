#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define BUF_SIZE 2056
#define _CRT_SECURE_NO_WARNINGS

void error_handling(char *message);

//  소켓을 생성해서 connect하고 recv해서 받아온걸 버퍼에 저장해서 보여주기

int main(int argc,char *argv[])
{
	int sock;
	char buf[BUF_SIZE];
	struct sockaddr_in serv_addr; // 연결할 서버의 주소
	int byteCount;
	FILE *fp = fopen("hw1.html","w");
	if(fp == NULL)
	{
		printf("파일을 열 수 없습니다.");
	}
	if(argc!=4)
	{
		printf("need ip & port\n");
		exit(1);
	}
	sock = socket(PF_INET,SOCK_STREAM,0);
	if(sock == -1)
	{
		error_handling("socket() error");
	}

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[2]);
	serv_addr.sin_port = htons(atoi(argv[3]));

	if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
	{
		error_handling("connect() error\n");
	}

	// send header
	char *header = "Get /webhp HTTP/1.1\r\nUser-Agent: Mozilla/4.0\r\ncontent-type:text/html\r\nConnection: close\r\n\r\n";
	send(sock,header,strlen(header),0);
	
	byteCount = recv(sock,buf,sizeof(buf),0);
	buf[byteCount] = 0; // 문자열 종료
		

	fprintf(fp,"%s",buf);
	
	fclose(fp);
	
	return 0;
}


void error_handling(char *message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
