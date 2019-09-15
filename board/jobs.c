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

#include "jobs.h"
#include "log.h"

void job_enqueue(struct job **head, void *data) {
    
    struct job *lp = *head;

    if (lp != NULL) {
	    while (lp->next != NULL)
	        lp = lp->next;
	    lp->next = (struct job *)malloc(sizeof(job));
	    lp = lp->next;
	    lp->next = NULL;
	    lp->data = data;
	    return;
    }
    else {
	    lp = (struct job *)malloc(sizeof(job));
	    lp->next = NULL;
	    lp->data = data;
        *head = lp;
	    return;
    }
}

void job_dequeue(struct job **head, void **data) {
    if (*head != NULL) {
        struct job *lp = *head;
        *data = lp->data;
        *head = lp->next;
        free(lp);
    } else {
        *data = NULL;
    }
    return;
}

void jobs_clean(struct job**head, free_data data_free) {
    while (*head != NULL) {
        struct job *lp = *head;
        data_free(lp->data);
        *head = (*head)->next;
        free(lp);
    }
    return;
}

void jobs_dump(struct job *head, dump_data data_dump) {

    if (head == NULL)
	    log_info("queue is empty!\n");
    else {
	    while (head != NULL) {
	        data_dump(head->data);
	        head = head->next;
	    }
    }
    log_info("\n");
}

