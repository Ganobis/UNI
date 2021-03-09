//Piotr Slowik 299786
//Zadanie nr 3 - transport

#include "functions.h"
//inicjalizacja ramek
void init(frame* frames, int frames_cnt, int amount){

    int i = 0;

    for(; i < frames_cnt-1; i++){
	    frames[i].start = i*FRAME_SIZE;
	    frames[i].len = FRAME_SIZE;
	    if(i==0){
		    frames[i].header_len = floor(log10((double)frames[i].len))+9;
	    }
	    else{
	        frames[i].header_len = floor(log10((double)frames[i].start))+floor(log10((double)frames[i].len))+9;
            }
	    frames[i].received = false;
    }

    frames[frames_cnt-1].start = i*FRAME_SIZE;
    frames[frames_cnt-1].len = amount - frames[frames_cnt-1].start;
    if(i==0){
		frames[i].header_len = floor(log10((double)frames[i].len))+9;
    }
    else{
        frames[frames_cnt-1].header_len = floor(log10((double)frames[frames_cnt-1].start))+floor(log10((double)frames[frames_cnt-1].len))+9;
    }
    frames[frames_cnt-1].received = false;
}


//wysylanie datagramu dla n-tej ramki
int send_datagram(int sockfd, struct sockaddr_in server_address, int n, frame* frames){

    char message[30];
    sprintf(message,"GET %d %d\n", frames[n].start, frames[n].len);
    int message_len = strlen(message);

    if (sendto(sockfd, message, message_len, 0, (struct sockaddr*) &server_address, sizeof(server_address)) != message_len) {
		fprintf(stderr, "sendto error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}


//odbieranie ready pakietow
int receive(int sockfd, frame* frames, int ready, char ip[20], int port){

    while(ready > 0){
        ready--;
        struct sockaddr_in  sender;
		socklen_t           sender_len = sizeof(sender);
		u_int8_t            buffer[IP_MAXPACKET+1];


		ssize_t datagram_len = recvfrom (sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);
		if (datagram_len < 0) {
			fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		char sender_ip_str[20];
		inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
		if(ntohs(sender.sin_port) != port || strcmp(sender_ip_str,ip)!=0){
			continue;
		}

		buffer[datagram_len] = 0;
        char header[20];
        sscanf((const char*) buffer, "%20[^\n]", header);
		int len = -1; int start = -1;
        sscanf(header+4, "%d %d", &start, &len);
		if(start < 0 || len < 0){
            printf("Trash info received, continuing\n");
			continue;
		}

		int idx = start/FRAME_SIZE;

        if(frames[idx].received){
			continue;
        }

        if(frames[idx].start!=start || frames[idx].len!=len){
            printf("Trash info received, continuing\n");
			continue;
        }

        frames[idx].received = true;
        frames[idx].text = (char*)malloc(sizeof(char)*frames[idx].len+1);
        memcpy(frames[idx].text,buffer+frames[idx].header_len, frames[idx].len);

    }
	return EXIT_SUCCESS;
}


//sprawdzenie czy mozemy przesunac ramke i zapisac do pliku
int check(int *curr_idx, int frames_cnt, frame* frames, FILE *f){

    while(*curr_idx < frames_cnt && frames[*curr_idx].received == true){
        if((int)(fwrite(frames[*curr_idx].text,sizeof(char),frames[*curr_idx].len,f)) != frames[*curr_idx].len){
            fprintf(stderr, "fwrite error: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        free(frames[*curr_idx].text);
        (*curr_idx)++;
        printf("%.2f%% completed\n", (double)(*curr_idx)/(double)frames_cnt*100);
    }
    return EXIT_SUCCESS;
}



int send_datagrams(int sockfd, struct sockaddr_in server_address, int frames_cnt, int curr_idx, frame * frames){

    for(int i = curr_idx; i < frames_cnt && i < curr_idx + FRAME_SIZE; i++){
        if(!frames[i].received){
	        if(send_datagram(sockfd, server_address, i, frames)!=0){
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;
}
