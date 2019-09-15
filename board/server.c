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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "io.h"
#include "udp_s.h"
#include "log.h"
#define MAXLINE 4096
static int log_enabled = 0;

static void error(char *msg)
{
    perror(msg);
    exit(1);
}

static void sig_handler(int signo)
{
    pid_t pid;
    int stat;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        fprintf(stderr, "child %d terminated\n", pid);
    return;
}

static void receive_file(int sockfd, char *file)
{
    int file_fd;
    char buf[MAXLINE];
    ssize_t size_read;
    ssize_t size_write;

    if ((file_fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) < 0) {
        fprintf(stderr, "open file %s failed\n", file);
        return;
    }

    while (1) {
        size_read = readn(sockfd, buf, MAXLINE);
        log_info("Readed %d bytes from socket\n", size_read);

        if (size_read == 0) {
            log_info("receive finished\n");
            break;
        } else if (size_read < 0) {
            fprintf(stderr, "receive failed\n");
            break;
        }

        size_write = writen(file_fd, buf, size_read);
        log_info("wrote %d bytes\n", size_write);
        if (size_write != size_read) {
            fprintf(stderr, "wrote failed\n");
            //break;
        }
    }
    close(file_fd);
    return;
}


int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    int sockfd, new_sockfd, portno;
    char *file;
    pid_t pid, pid_udp;

    if (argc < 4) {
        fprintf(stderr, "Usage:%s <tcp_port> <filepath> <udp_port> [log]\n", argv[0]);
        exit(1);
    }

    if (argc == 5) {
        log_enabled = atoi(argv[4]);
    }

    log_init(log_enabled);
    
    pid_udp = fork();
    if (pid_udp == 0) { /* UDP process */
        udp_work(argv[2], argv[3]);
    }

    file = argv[2];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to open socket\n");
        exit(1);
    }

    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*) &server_addr,
                sizeof(server_addr)) < 0) {
        error("Error on binding\n");
    }
    
    listen(sockfd, 5);
    client_len = sizeof(client_addr);

    signal(SIGCHLD, sig_handler);

    while(1) {
        new_sockfd = accept(sockfd, (struct sockaddr*) &client_addr, &client_len);
        if ( new_sockfd < 0) {
            if (errno == EINTR)
                continue;
            else
                error("Error on accept\n");
        }

        pid = fork();
        
        if (pid < 0) {
            error("Error on fork\n");
        }
        else if (pid == 0) { /* Child process */
            close(sockfd);
            receive_file(new_sockfd, file);
            exit(0);
        } else {
            close(new_sockfd);
        }
    }
    close(sockfd);
    return 0;
}

