#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
void error_handling(char *message);


int main(int argc, char *argv[]){
	struct hostent *host;
	int i;
	
	if(argc!=2)
	{
		fprintf(stderr,"cannot find ip address from %s\n",argv[0]);
		exit(1);
	}
	host = gethostbyname(argv[1]);

	printf("official name: %s\n", host->h_name);
	for(i=0;host->h_aliases[i];i++)
	{
		printf("aliases %d: %s\n",i+1,host->h_aliases[i]);
	}
	printf("address type : %s\n", (host->h_addrtype == PF_INET) ? "PF_INET":"Unknown");

	for(i=0;host->h_addr_list[i];i++)
	{
		printf("IP addr %d : %s\n",i+1,inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
	}
	return 0;
}
