#include <stdio.h>
#include <errno.h>
#include <syslog.h>
#include <stdarg.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BASE1970		(2208988800L)
#define DBG(...) fprintf(stderr, "<TOD> %s(),L%d: ", __FUNCTION__, __LINE__);printf(__VA_ARGS__)



void show_help()
{
    printf( "Usage:\n \
            if executed without arguments it will use the default setting\n \
            -h prints this help\n \
            -i swich between ipv4 and ipv6 (default is ipv4)\n \
            -t use tcp as protocol (default is udp)\n \
            -p [port] use different working port (default is 37)\n \
            -d enable debugging output (default is off)\n ");
    return;
}

void UDP_Sendtime(sa_family_t IP, int oflag, int port)
{
	long rfc_time;
    int socketfd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t client_length;
	int ret;
	
    if(oflag){
        DBG("starting rfc868 server...\n using protocol: UDP \n using port: %d\n",port);
	}

    socketfd = socket(IP, SOCK_DGRAM, 0);
    if(socketfd < 0)
    {
        DBG("ERROR while opening socket!");
        exit(-1);
    }
    server_addr.sin_family = IP;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        DBG("ERROR while binding!");
	}
    if(oflag){
       DBG("waiting on port %d\n", port);
	}
    client_length = sizeof(struct sockaddr_in);
    char buf[1];
    while(1)
    {
		//Recieve fake data ('a') to get the destination adress structure
		int n = recvfrom(socketfd, buf, 1, 0, (struct sockaddr *) &client_addr, &client_length);
		if(n < 0){
			DBG("Error while recieving!");
		}
		if(oflag){
			DBG("client connected: %s\n",inet_ntoa(client_addr.sin_addr));
		}
		rfc_time =htonl(time(0) + 2208988800L);
		ret = sendto(socketfd, &rfc_time, sizeof(rfc_time), 0, (struct sockaddr *)&client_addr, client_length);
		if(ret < 0){
			DBG("ERROR while sending time!");
		}
		if(oflag){
			DBG("time sent (%lu) seconds since 1.1.1900\n",rfc_time);
		}
    }
	close(socketfd);
}

void TCP_Sendtime(sa_family_t IP, int oflag, int port)
{
	long rfc_time;
    int socketfd;
    int newsockfd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t client_length;
	int ret;
	
    if(oflag){
        DBG("starting rfc868 server...\nusing protocol: TCP\nusing port: %d\n",port);
	}
    socketfd = socket(IP, SOCK_STREAM, 0);
    if(socketfd < 0)
    {
        DBG("ERROR while opening socket!");
        exit(-1);
    }
    server_addr.sin_family = IP;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        DBG("ERROR while binding!");
	}
    if(oflag){
        DBG("listening on port %d....\n",port);
	}
    listen(socketfd, 5);
	
    while(1)
    {
        client_length = sizeof(client_addr);
        newsockfd = accept(socketfd, (struct sockaddr*) &client_addr, &client_length);
        if(newsockfd < 0)
        {
            DBG("ERROR while accepting!");
        }
        if(oflag){
            DBG("client connected: %s\n",inet_ntoa(client_addr.sin_addr));  
		}			
        rfc_time =htonl(time(0) + 2208988800L);

        ret = write(newsockfd, &rfc_time, sizeof(rfc_time));

        if(ret < 0){
            DBG("ERROR while writing to socket!");
		}
        if(oflag){
            DBG("time sent (%lu) seconds since 1.1.1900\n",rfc_time);
		}
        close(newsockfd);
    }
	close(socketfd);
}


int main(int argc, char* argv[])
{
    int c;
    //Set up the default settings
    int oflag = 0;
    int ipflag =0;
    int udpflag = 1;
    int portflag = 37;

    sa_family_t ip;
    optarg = NULL;
    while((c = getopt(argc, argv, "ditp:h")) != -1)
        switch(c)
        {
        case 'd':
            oflag = 1;
            break;
        case 'i':
            ipflag = 1;
            break;
        case 't':
            udpflag = 0;
            break;
        case 'p':
            portflag = atoi(optarg);
            break;
        case 'h':
            show_help();
			return 0;
        default:
            show_help();
			return 0;
        }

    if(ipflag)
        ip = AF_INET6;
    else
        ip = AF_INET;
    if(udpflag)
    {
        UDP_Sendtime(ip, oflag, portflag);
    }
    else
    {
        TCP_Sendtime(ip, oflag, portflag);
    }
    return 0;
}

