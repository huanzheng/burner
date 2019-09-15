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

#ifndef JOBS_H
#define JOBS_H

#include <stdlib.h>
#include <stdio.h>

typedef struct job {
    struct job *next;
    void  *data;
}job;

typedef void free_data(void *data);
typedef void dump_data(void *data);

void job_enqueue(struct job **head, void *data);
void job_dequeue(struct job **head, void **data);
void jobs_clean(struct job **head, free_data data_free);
void jobs_dump(struct job *head, dump_data data_dump);

#endif
