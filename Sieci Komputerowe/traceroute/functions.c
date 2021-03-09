//Piotr Słowik
//299786
//traceroute
#include "functions.h"

#define ID_OFFSET 32
#define SEQ_OFFSET 34

void init(){

	for(int i = 0; i < 30; i++){
		how_many[i]=0;
	}

}

float avg_time(int ttl){
	return (time[ttl-1][0] + time[ttl-1][1] + time[ttl-1][2]) / 3.0;
}


//Ekstraktuje wartosc lower endian
int get_le_val(unsigned char * buff){
	return (int)(*buff) + ((int)(*(buff+1))/16)*16*16*16 + ((int)(*(buff+1))%16)*16*16;
}


u_int16_t compute_icmp_checksum (const void *buff, int length)
{
	u_int32_t sum;
	const u_int16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}


//Wysyla icmp echo o ttl ttl, id id, sockecie sockfd na adres target
int send_ttl(int ttl, int id, int sockfd, char *target){

	struct icmp header;
	header.icmp_type = ICMP_ECHO;
	header.icmp_code = 0;
	header.icmp_hun.ih_idseq.icd_id = id;
	header.icmp_hun.ih_idseq.icd_seq = ttl;
	header.icmp_cksum = 0;
	header.icmp_cksum = compute_icmp_checksum ((u_int16_t*)&header, sizeof(header));

	struct sockaddr_in recipient;
	bzero (&recipient, sizeof(recipient));
	recipient.sin_family = AF_INET;
	inet_pton(AF_INET, target, &recipient.sin_addr);
	setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
	ssize_t bytes_sent = sendto (  sockfd,  &header,   sizeof(header),  0,   (struct sockaddr*)&recipient,   sizeof(recipient));

	if (bytes_sent < 0) {
		fprintf(stderr, "sendto error: %s\n", strerror(errno)); 
		return -1;
	}

	return 0;
}


//Przyjmuje ready pakietow w gniezdzie sockfd i jesli pasuja do naszego seq to przypisujemy im czas w zaleznosci od micro_s uplynietych na czekaniu
//Przerobiona funkcja z wykladu
int receive_packets(int ready, int sockfd, long long micro_s)
{

	while (ready > 0) {
		
		struct sockaddr_in 	sender;	
		socklen_t 			sender_len = sizeof(sender);
		u_int8_t 			buffer[IP_MAXPACKET];

		ssize_t packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);
		if (packet_len < 0) {
			fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
			return 1;
		}

		char sender_ip_str[20]; 
		inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
		

		struct ip* 			ip_header = (struct ip*) buffer;
		ssize_t				ip_header_len = 4 * ip_header->ip_hl;

		u_int8_t* icmp_packet = buffer + 4 * ip_header->ip_hl;
		struct icmp* icmp = (struct icmp*) icmp_packet;

		if(icmp->icmp_type == 11){

			if(current_ttl == get_le_val(buffer + ip_header_len + SEQ_OFFSET) && pid == get_le_val(buffer + ip_header_len + ID_OFFSET)){
				strcpy(ips[current_ttl-1][how_many[current_ttl-1]],sender_ip_str);
				time[current_ttl-1][how_many[current_ttl-1]] = (1000000-micro_s)/1000.0;
				how_many[current_ttl-1]+=1;
			}

		}

		if(icmp->icmp_type == 0){

			if(icmp->icmp_seq == current_ttl){
				strcpy(ips[current_ttl-1][how_many[current_ttl-1]],sender_ip_str);
				time[current_ttl-1][how_many[current_ttl-1]] = (1000000-micro_s)/1000.0;
				how_many[current_ttl-1]+=1;
				receiving = false;
			}

		}

		ready--;

	}

	return 0;

}

void print_ttl(int ttl){

	if (how_many[ttl-1] == 3){

		if(strcmp(ips[ttl-1][0],ips[ttl-1][1])==0 && strcmp(ips[ttl-1][2],ips[ttl-1][1])==0){
			printf("%d %s ", ttl, ips[ttl-1][0]);
		}

		else if(strcmp(ips[ttl-1][0],ips[ttl-1][1])==0 && strcmp(ips[ttl-1][2],ips[ttl-1][1])==1){
			printf("%d %s %s ", ttl, ips[ttl-1][0], ips[ttl-1][2]);
		}

		else if(strcmp(ips[ttl-1][0],ips[ttl-1][2])==0 && strcmp(ips[ttl-1][2],ips[ttl-1][1])==1){
			printf("%d %s %s ", ttl, ips[ttl-1][0], ips[ttl-1][1]);
		}

		else{
			printf("%d %s %s %s ", ttl, ips[ttl-1][0], ips[ttl-1][1], ips[ttl-1][2]);
		}
		printf("%.2fms\n", avg_time(ttl));
	}

	else if (how_many[ttl-1] == 2){

		if(strcmp(ips[ttl-1][0],ips[ttl-1][1])==0){
			printf("%d %s ???\n", ttl, ips[ttl-1][0]);
		}

		else{
			printf("%d %s %s ???\n", ttl, ips[ttl-1][0], ips[ttl-1][1]);
		}

	}
	else if (how_many[ttl-1] == 1){
		printf("%d %s ???\n", ttl, ips[ttl-1][0]);
	}

	else{
		printf("%d *\n", ttl);
	}

}

