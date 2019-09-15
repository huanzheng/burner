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

#ifndef UDP_WORK_H
#define UDP_WORK_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

typedef struct udp_info {
    int sock_fd;
    struct sockaddr_in from_addr;
    socklen_t from_len;
    char *buf;
    int buf_len;
} udp_info;

typedef struct cmd_arg {
    char *image_path;
    udp_info *u_i;
} cmd_arg;

typedef void udp_msg_back(char *msg, int msg_len, udp_info *u_i);
typedef void cmd_handler(struct cmd_arg *arg, udp_msg_back callback);

void udp_work(char *filepath, char *udp_port);

#endif
