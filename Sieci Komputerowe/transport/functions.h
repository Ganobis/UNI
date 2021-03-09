//Piotr Slowik 299786
//Zadanie nr 3 - transport

#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>
#define MAX_FRAMES 5
#define FRAME_SIZE 1000

typedef struct frame{

    int start;
    int len;
    int header_len;
    bool received;
    char * text;

}frame;

void init(frame* frames, int frames_cnt, int amount);
int send_datagram(int sockfd, struct sockaddr_in server_address, int n, frame* frames);
int receive(int sockfd, frame* frames, int ready, char ip[20], int port);
int check(int *curr_idx, int frames_cnt, frame* frames, FILE *f);
int send_datagrams(int sockfd, struct sockaddr_in server_address, int frames_cnt, int curr_idx, frame * frames);
#endif
