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
#include <sys/errno.h>
#include <pthread.h>
#include <assert.h>

#include "jobs.h"
#include "burn.h"
#include "log.h"
#include "udp_s.h"

#define BUFSIZE 512
#define CMDSIZE 64
#define CMD_STOP "stop"
#define CMD_BURN "burn"

typedef struct cmd_item {
    char *cmd;
    cmd_handler *handler;
} cmd_item;

struct cmd_item cmd_map[] = {
    {CMD_BURN, burn},
    {NULL, NULL}
}; 

static struct sockaddr_in udp_server_addr, udp_from_addr;
static int udp_sockfd;

static pthread_mutex_t jobs_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  jobs_available = PTHREAD_COND_INITIALIZER;
static int jobs_count = 0;
static job *jobs_head = NULL;
static char image_path[BUFSIZE];

static void* process_msg(void *arg);
static void* receive_msg(void *arg);
static void print_sockaddr(struct sockaddr_in sa);

static void free_udp_info(void *data)
{
    struct udp_info *u_i = (udp_info*) (data);
    free(u_i->buf);
    free(u_i);
}

static void dump_udp_info(void *data)
{
    char buf[CMDSIZE];
    struct udp_info *u_i = (udp_info*) (data);

    print_sockaddr(u_i->from_addr);
    log_info("udp buf length is %d\n", u_i->buf_len);
    if (u_i->buf_len < CMDSIZE) {
        memcpy(buf, u_i->buf, u_i->buf_len);
        buf[u_i->buf_len] = '\0';       
        log_info("cmd is %s\n", buf);
    } else {
        log_info("cmd too long to be displayed\n");
    }
}

static int buf_is_cmd(char *buf, char *cmd, int buf_len)
{
    assert(buf);
    assert(cmd);
    int i = 0;
    int cmd_len = strlen(cmd);

    /* check whether buf starts with cmd */
    if (buf_len < cmd_len)
        return 0;

    while (i < buf_len && i < cmd_len) {
        if (buf[i] != cmd[i])
            return 0;
        i++;
    }
    return 1;
}

static void error(char *msg)
{
    perror(msg);
    exit(1);
}

static void print_sockaddr(struct sockaddr_in sa)
{
    log_info("sa = %d, %s, %d\n", sa.sin_family,
        inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
}

static void msg_back(char *msg, int msg_len, udp_info *u_i)
{
    int n;
    n = sendto(u_i->sock_fd, msg, msg_len,
            0, (struct sockaddr*) &u_i->from_addr, u_i->from_len);
    if (n < 0) {
        fprintf(stderr, "msg back failed\n");
    }
}

static void* receive_msg(void *arg)
{
    fprintf(stderr, "Receive thread created\n");
    char buf[BUFSIZE];
    int n;
    socklen_t from_len;
    from_len = sizeof(udp_from_addr); /* fuck, this is extreamly important */

    while (1) {
        bzero(buf, BUFSIZE);
        if ((n = recvfrom(udp_sockfd, buf, BUFSIZE, 0,
            (struct sockaddr *)&udp_from_addr, &from_len)) <= 0) {
            fprintf(stderr, "Receive failed\n");
            fprintf(stderr, "Error message %s\n", strerror(errno));
            continue;
        }
        log_info("Received %d bytes\n", n);
        if (n < BUFSIZE) {
            buf[n] = '\0';
            log_info("Received %s\n", buf);
        }
        print_sockaddr(udp_from_addr);

        udp_info *data = (udp_info*) malloc(sizeof(udp_info));
        bcopy(&udp_sockfd, &data->sock_fd, sizeof(udp_sockfd));
        bcopy(&udp_from_addr, &data->from_addr, sizeof(udp_from_addr));
        bcopy(&from_len, &data->from_len, sizeof(from_len));
        data->buf = (char*) malloc(n);
        bzero(data->buf, n);
        bcopy(buf, data->buf, n);
        data->buf_len = n;

        /* Just put the msg into job queue if it is not stop */
        if (buf_is_cmd(data->buf, CMD_STOP, data->buf_len)) {
            pthread_mutex_lock(&jobs_queue_mutex);
            jobs_clean(&jobs_head, free_udp_info);
            jobs_count = 0;
            pthread_mutex_unlock(&jobs_queue_mutex);
        } else {
            pthread_mutex_lock(&jobs_queue_mutex);
            job_enqueue(&jobs_head, (void *)data);
            jobs_count++;
            pthread_cond_signal(&jobs_available);
            pthread_mutex_unlock(&jobs_queue_mutex);
        }
    }
    fprintf(stderr, "!!!!!Receive thread exits\n");
}

void udp_work(char *filepath, char *udp_port)
{
    pthread_t job_receiver, job_processer;

    strncpy(image_path, filepath, strlen(filepath) + 1);
    udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sockfd < 0)
        error("Failed to create socket\n");

    bzero(&udp_server_addr, sizeof(udp_server_addr));
    bzero(&udp_from_addr, sizeof(udp_from_addr));

    udp_server_addr.sin_family = AF_INET;
    udp_server_addr.sin_addr.s_addr = INADDR_ANY;
    udp_server_addr.sin_port = htons(atoi(udp_port));
    
    if (bind(udp_sockfd, (struct sockaddr *)&udp_server_addr, sizeof(udp_server_addr)) < 0)
        error("Bind socket failed\n");
    
    pthread_create(&job_processer, NULL, process_msg, NULL);
    pthread_create(&job_receiver, NULL, receive_msg, NULL);

    pthread_join(job_processer, NULL);
    pthread_join(job_receiver, NULL);
}

static void* process_msg(void *arg)
{
    fprintf(stderr, "Processor thread created\n");
    udp_info *u_i;

    while(1) {
        pthread_mutex_lock(&jobs_queue_mutex);
        while(jobs_count == 0)
            pthread_cond_wait(&jobs_available, &jobs_queue_mutex);
        /* wake up to get a job from queue*/
        log_info("Processer Wake Up\n");
        //jobs_dump(jobs_head, dump_udp_info);
        job_dequeue(&jobs_head, (void**)&u_i);
        jobs_count--;
        pthread_mutex_unlock(&jobs_queue_mutex);
    
        cmd_arg arg;
        arg.image_path = image_path;
        arg.u_i = u_i;

        int i = 0;
        while (cmd_map[i].cmd != NULL) {
            if (buf_is_cmd(u_i->buf, cmd_map[i].cmd, u_i->buf_len))
                break;
            i++;
        }
        /* Got the cmd item, execute it */
        if (cmd_map[i].cmd != NULL)
            cmd_map[i].handler(&arg, msg_back);

        free_udp_info((void *)u_i);
    }
    fprintf(stderr, "!!!!!Processor thread exits\n");
}

