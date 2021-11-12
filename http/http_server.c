#include <stdio.h>
#include <signal.h> // zombie
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define NOT_FOUND_CONTENT "<h1>404 Not Found</h1>\n"
#define HEADER "HTTP/1.1 %d %s\nContent-Length: %ld\nContent-Type: %s\n\n"




void server_process(int clnt_sock);


int main(int argc, char *argv[])
{
	int port;
	int pid; // child
	int serv_sock;
	int clnt_sock;
	socklen_t adr_sz;

	// 현재 동작중인 컴퓨터를 사용하기 위해 inaddr any 고려하기
	
	if(argc !=2)
	{
		printf("invalid input");
		exit(0);
	}

	// save port
	port = atoi(argv[1]);

	// create server sock
	serv_sock = socket(PF_INET,SOCK_STREAM,0);
	if(serv_sock < 0)
	{
		printf("create sock error");
		exit(1);
	}

	//bind
	// 80포트는 당연히 자주 사용되는거니까 bind 실패 다른걸로
	struct sockaddr_in sin;
	struct sockaddr_in clnt_addr;
	memset(&sin,0,sizeof(sin));
	sin.sin_family = PF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port= htons(port);

	if(bind(serv_sock,(struct sockaddr*)&sin,sizeof(sin)) ==-1)
	{
		printf("binding fail");
		exit(1);
	}


	//listen
	if(listen(serv_sock,10) ==-1) // backlog 10 설정
	{
		printf("fail to listen");
		exit(1);
	}


	// accept는 fork딴 자식으로 수행 고려
	// zombie 처리 과정 추가
 	signal(SIGCHLD,SIG_IGN);	
	while(1)
	{
		adr_sz = sizeof(clnt_addr);
		printf("hello\n");
		clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&adr_sz);
		if(clnt_sock==-1)
		{
			printf("accept error");
			exit(1);
		}

		printf("befor\n");
		pid = fork();
		if(pid == 0)
		{
			printf("fork\n");
			// child
			close(serv_sock); // 필요없으니 닫는게 좋다고 했다
			// 구현
			server_process(clnt_sock);

			close(clnt_sock);
			return 0;
		}else{
			close(clnt_sock); //부모는 다시 할 일 하러
		}
	}

}



void server_process(int clnt_sock)
{
	char buffer[1000];
	int str_len = read(clnt_sock,buffer,1000);
	if(str_len ==0)
	{
		return;
	}
	//finish
	buffer[1000] = 0;
	// uri 추출
	char *method = strtok(buffer," "); // method뽑는데 어차피 get??  " " -> \0
	char *uri = strtok(NULL," "); // uri 추출
	if(uri == NULL)
	{
		printf("invalid uri");
		return;
	}

	// 파일이 있는지 확인
	// uri = ex) /index.html 앞에 /은 불필요하니까
	char copiedUri[1000];
	strcpy(copiedUri,uri);
	char *uri_index = copiedUri + 1;


	printf("here1\n");


	// 해당 파일 정보 상태확인 만약 -1오면 없는거니까
	struct stat state;
	if(stat(uri_index,&state) == -1)
	{
		// 404 ERROR
		// header
		char header[1000];
		long size = sizeof(NOT_FOUND_CONTENT);
		sprintf(header,HEADER,404,"NOT FOUND",size,"text/html");

		write(clnt_sock,header,strlen(header));
		write(clnt_sock,NOT_FOUND_CONTENT,sizeof(NOT_FOUND_CONTENT));
	}

	int fd = open(uri_index,O_RDONLY);
	if(fd == -1)
	{
		printf("cannot open file");
		return;
	}

	printf("here2\n");

	// content type 변경
	// 127.0.0.1/index.html -> type = text/html
	char content_type[40];
	char *ext = strrchr(uri_index,'.');
	if(!strcmp(ext,".html")) // same
	{
		strcpy(content_type,"text/html");
	}else if(!strcmp(ext,".jpg"))
	{
		strcpy(content_type,"image/.jpeg");
	}else if(!strcmp(ext,".png"))
	{
		strcpy(content_type,"image/.png");
	}else{
		printf("invalid contet type");
		return;
	}
	char header[1000];
	long content_length = state.st_size;
	sprintf(header,HEADER,200,"OK",content_length,content_type);
	write(clnt_sock,header,strlen(header));

	int count;
	while((count = read(fd,buffer,1000)) > 0)
	{
		write(clnt_sock,buffer,count);
	}

}
