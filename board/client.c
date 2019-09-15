/*
   Copyright 2010, 2011 Huan Zheng <huan.zheng.ray@gmail.com> 

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General
   Public License along with this library; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301 USA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUFSIZE 256
static struct sockaddr_in server_addr, from_addr;
static int sockfd, portno;
static char msg[256];

void error(char *msg)
{
    perror(msg);
    exit(-1);
}

static void* receive_msg(void *arg)
{
    printf("receive thread created\n");
    char buf[BUFSIZE];
    int n;
    socklen_t fromlen;

    fromlen = sizeof(from_addr);
    while (1) {
        bzero(buf, BUFSIZE);

        if ((n = recvfrom(sockfd, buf, BUFSIZE, 0,
            (struct sockaddr *)&from_addr, &fromlen)) < 0) {
            printf("Receive failed\n");
        }
        printf("Received %d bytes\n", n);
        write(1, buf, n);
        printf("\n");
    }
}

static void* send_msg(void *arg)
{
    int n;
    socklen_t serlen;
    char buf[1];
    int i = 0;

    serlen = sizeof(server_addr);
    /*
    while (i < 10) {
        sleep(1);
        buf[0] = '0' + i;
        n = sendto(sockfd, buf, 1, 0, (struct sockaddr *)&server_addr, serlen);
        if (n < 0) {
            printf("Send msg failed\n");
        } else {
            printf("Send msg succeeded, %d bytes of value %d sent\n", n, i);
        }
        i++;
    }*/
    while (i < 10) {
        char buf[20];
        bzero(buf, 20);
        
        int l = strlen(msg);
        memcpy(buf, msg, l);

        buf[l] = '_';
        buf[l+1] = '0' + i;
        printf("Send Msg %s\n", buf);

        n = sendto(sockfd, buf, l+2, 0, (struct sockaddr *)&server_addr, serlen);
        if (n < 0) {
            printf("Send msg failed\n");
        } else {
            printf("Send msg succeeded, %d bytes of value %s sent\n", n, msg);
        }
        i++;
    }
}

static void* send_stop_msg(void *arg)
{
    int n;
    socklen_t serlen;

    serlen = sizeof(server_addr);
    printf("Send Stop Msg\n");
    n = sendto(sockfd, "stop", 4, 0, (struct sockaddr *)&server_addr, serlen);
    if (n < 0) {
        printf("Send msg failed\n");
    } else {
        printf("Send stop msg succeeded\n");
    }
}

int main(int argc, char *argv[])
{
    pthread_t r, s, send_stop;

    if (argc < 4) {
        printf("Usage: %s <server ip> <port> <msg>\n", argv[0]);
        return;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("Failed to create socket\n");

    bzero(&server_addr, sizeof(server_addr));
    bzero(&from_addr, sizeof(from_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));
    strncpy(msg, argv[3], strlen(argv[3]) + 1);
    
   // if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
     //   error("Bind socket failed\n");
    
    pthread_create(&r, NULL, receive_msg, NULL);
    pthread_create(&s, NULL, send_msg, NULL);

    sleep(3);
    pthread_create(&send_stop, NULL, send_stop_msg, NULL);

    pthread_join(r, NULL);
    pthread_join(s, NULL);
    pthread_join(send_stop, NULL);
}
