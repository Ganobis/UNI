//Piotr Słowik
//299786
//traceroute
#include "functions.h"

bool receiving = true;
int current_ttl = 1;
char ips[30][3][20];
float time[30][3];
int how_many[30];
int pid;

int main(int argc, char** argv){

	init();
	pid = getpid();

	if(argc == 1){
		printf("Error, no argument");
		printf("Usage: /.traceroute ip_addr");
		exit(EXIT_FAILURE);
	}

	printf("Traceroute to %s:\n", argv[1]);
	printf("TTL | IPS | AVG-TIME\n");


	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	if (sockfd < 0) {
		fprintf(stderr, "socket error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}
	
	for (int i = 1; i<=30 && receiving; i++){
		
		assert(send_ttl(i, pid, sockfd, argv[1]) == 0);
		assert(send_ttl(i, pid, sockfd, argv[1]) == 0);
		assert(send_ttl(i, pid, sockfd, argv[1]) == 0);

		fd_set descriptors;
		FD_ZERO (&descriptors);
		FD_SET (sockfd, &descriptors);
		struct timeval tv; tv.tv_sec = 1; tv.tv_usec = 0;

		while(how_many[current_ttl-1]!=3 && (tv.tv_usec >0 || tv.tv_sec == 1)){
			int ready = select(sockfd+1, &descriptors, NULL, NULL, &tv);
			if(ready < 0){
				fprintf(stderr, "select error: %s\n", strerror(errno)); 
				return EXIT_FAILURE;
			}
			assert(receive_packets(ready, sockfd,tv.tv_usec) == 0);
		}

		print_ttl(current_ttl);
		current_ttl++;
	}
	
	return EXIT_SUCCESS;

}
