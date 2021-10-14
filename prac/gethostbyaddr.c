#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

void error_handling(char *message);

int main(int argc, char *argv[]){
	struct hostent *host;
	struct sockaddr_in addr;
	int i;
	
	if(argc!=2)
	{
		printf("usage : %s <ip>\n",argv[0]);
		exit(1);
	}
	memset(&addr,0,sizeof(addr));

	if(!inet_aton(argv[1],&addr.sin_addr))
	{
		error_handling("convergetn error\n");
	}


	host = gethostbyaddr((char *)&addr.sin_addr,4,AF_INET);
	if(!host)
	{
		error_handling("gethost error");
	}
	printf("official name : %s\n",host->h_name);

	for(i=0;host->h_aliases[i];i++)
	{
		printf("aliases %d: %s\n",i+1,host->h_aliases[i]);
	}

	printf("address type: %s \n",(host->h_addrtype==AF_INET)?"AF_INET":"AF_INET6");

	for(i=0;host->h_addr_list[i];i++)
	{
		printf("ip addr %d: %s \n",i+1,inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
	}
	return 0;
}
	
void error_handling(char *message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
