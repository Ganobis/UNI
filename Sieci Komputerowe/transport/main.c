//Piotr Slowik 299786
//Zadanie nr 3 - transport

#include "functions.h"

int main(int argc, char **argv)
{
	if(argc < 5){
		printf("Usage: %s ip port filename size", argv[0]);
		return EXIT_FAILURE;
	}
	int amount = atoi(argv[4]);
	int frames_cnt = ceil(amount/1000.0);
    	int curr_idx = 0;
	int port = atoi(argv[2]);
	char ip[20] = "";
	strcpy(ip, argv[1]);
	frame *frames = (frame*)malloc(sizeof(frame)*frames_cnt);
	init(frames,frames_cnt,amount);

	FILE *f;
	if((f = fopen(argv[3], "wb")) == NULL){
		fprintf(stderr, "fopen error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "socket error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}

	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(atoi(argv[2]));
	if(inet_pton(AF_INET, argv[1], &server_address.sin_addr)!=1){
		fprintf(stderr, "inet_pton error: %s\n", strerror(errno)); 
		printf("Usage: %s ip port filename size", argv[0]);
		return EXIT_FAILURE;
	}

    	while(curr_idx < frames_cnt){

        	assert(send_datagrams(sockfd, server_address, frames_cnt, curr_idx, frames)==0);

        	fd_set descriptors;
		FD_ZERO (&descriptors);
		FD_SET (sockfd, &descriptors);
		struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 50000;

		while(tv.tv_usec >0){
			int ready = select(sockfd+1, &descriptors, NULL, NULL, &tv);
			if(ready < 0){
				fprintf(stderr, "select error: %s\n", strerror(errno)); 
				return EXIT_FAILURE;
			}
            		if(ready > 0){
			    assert(receive(sockfd, frames, ready, ip, port) == 0);
               		    check(&curr_idx, frames_cnt, frames, f);
            		}
		}
    	}
	close (sockfd);
	fclose(f);
	printf("Download completed!\n");
	return EXIT_SUCCESS;

}

	
