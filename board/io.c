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
#include <unistd.h>
#include <sys/errno.h>

ssize_t readn(int fd, void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr = vptr;
    nleft = n;

    while(nleft > 0) {
        nread = read(fd, ptr, nleft);

        if (nread < 0) {
            if (errno == EINTR) {
                nread = 0;
            } else {
                return (-1);
            }
        } else if (nread == 0) {
            break;                     /* EOF */
        }

        ptr += nread;
        nleft -= nread;
    }
    return (n-nleft);
}

ssize_t writen(int fd, void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    char *ptr;

    ptr = vptr;
    nleft = n;

    while (nleft > 0) {
        nwritten = write(fd, ptr, nleft);

        if (nwritten < 0) {
            if (errno == EINTR) {
                nwritten = 0;
            } else {
                return (-1);
            }
        } 
        
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}
