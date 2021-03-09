//Piotr Słowik
//299786
//traceroute
#ifndef FUNCTION_H
#define FUNCTION_H

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip_icmp.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#define ID_OFFSET 32
#define SEQ_OFFSET 34

extern bool receiving;
extern int current_ttl;
extern char ips[30][3][20];
extern float time[30][3];
extern int how_many[30];
extern int pid;

void init();

float avg_time(int ttl);
int get_le_val(unsigned char * buff);
u_int16_t compute_icmp_checksum (const void *buff, int length);
int send_ttl(int ttl, int id, int sockfd, char *target);
int receive_packets(int ready, int sockfd, long long micro_s);
void print_ttl(int ttl);
#endif 
