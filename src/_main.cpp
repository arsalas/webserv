/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

#include <poll.h>
void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[9000];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	if (argc < 2)
	{
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char *)&serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	// Vincula un socket a una direccion
	printf("1\n");
	if (bind(sockfd, (struct sockaddr *)&serv_addr,
			 sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	// Se queda escuchando un request
	listen(sockfd, 5);
	printf("2\n");

	struct sockaddr_in serv_addr2, cli_addr2;
	int sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd2 < 0)
		error("ERROR opening socket");
	bzero((char *)&serv_addr2, sizeof(serv_addr2));
	serv_addr2.sin_family = AF_INET;
	serv_addr2.sin_addr.s_addr = INADDR_ANY;
	serv_addr2.sin_port = htons(3000);
	// Vincula un socket a una direccion
	printf("1\n");
	if (bind(sockfd2, (struct sockaddr *)&serv_addr2,
			 sizeof(serv_addr2)) < 0)
		error("ERROR on binding");
	// Se queda escuchando un request
	listen(sockfd2, 5);

	printf("2\n");
	struct pollfd pollfds[2];
	pollfds[0].fd = sockfd;
	printf("fd: %i\n", pollfds[0]);
	pollfds[0].events = POLLIN | POLLPRI;
	pollfds[1].fd = sockfd2;
	printf("fd: %i\n", pollfds[1]);
	pollfds[1].events = POLLIN | POLLPRI;
	int pollResult = poll(pollfds, 2, 5000000);

	printf("poll: %i\n", pollResult);

	clilen = sizeof(cli_addr);
	// AQUI ES CUANDO RECIVE LA PETICION
	for (size_t i = 0; i < 2; i++)
	{
		if (pollfds[i].revents & POLLIN)
		{
			printf("Es este %i\n", i);

			newsockfd = accept(pollfds[i].fd,
							   (struct sockaddr *)&cli_addr,
							   &clilen);
			printf("3\n");
			if (newsockfd < 0)
				error("ERROR on accept");
			bzero(buffer, 9000);
			n = read(newsockfd, buffer, 8999);
			if (n < 0)
				error("ERROR reading from socket");
			printf("Here is the message: %s\n", buffer);
			n = write(newsockfd, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 101 \n\n<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>", 163);
			if (n < 0)
				error("ERROR writing to socket");
			close(newsockfd);
			close(sockfd);
		}
	}
	return 0;
}