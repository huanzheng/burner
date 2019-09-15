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

#include "udp_s.h"
#include "log.h"
#include "msg_back_utils.h"

/* NOTE:
 * The progress sent back should be like cmd received plus _p_XX, 
 * say CMD_p_XX, E.g. burn_40_p_99
 * The succeed/fail sent back should be cmd received plus _s/_f, 
 * say CMD_XX_s/CMD_XX_f, E.g. burn_40_s/burn_40_f
 * */

#define SUCCEEDED_SUFIX "_s"
#define FAILED_SUFIX "_f"
#define PROGRESS_MID "_p_"

typedef struct prgres
{
    int step_size;
    int emitted;
} prgres;

static struct prgres progress[PROGRESS_STEPS];
static int p_steps;

/* 
 * @brief Initialize the prgres array
 * @param total, Image size
 * @param steps, Number of progress msgs
 * */
void progress_helper_init(int total, int steps)
{
    p_steps = steps;
    int step_size = (int)(((double)(total))/steps);
    int i = 0;
    log_info("Progress helper init, total size is %d, "
            "step size is %d\n", total, step_size);
    
    for (i = 0; i < p_steps; i++) {
        progress[i].step_size = step_size * i;
        progress[i].emitted = 0;
    }
}

/* 
 * NOTE: message back size, not pecentage, to avoid calculation on board
 * pecentage need to be calculated out on host machine
 * @brief function to send progress UDP message back
 * @param done, size already finished
 * @param arg, parameter passed from outside
 * @param msg_back, callback function to send UDP msg to host
 * */
void progress_back(int done, struct cmd_arg *arg, udp_msg_back msg_back)
{
    int i;

    for (i = 0; i < p_steps; i++) {
        if (progress[i].emitted == 0)
            break;
    }
    
    if (i < p_steps) {
        if (done >= progress[i].step_size && progress[i].emitted == 0) {
            char num[256];
            char msg[512];
            bzero(num, 256);
            bzero(msg, 512);
            int msg_len = 0;

            sprintf(num, "%d", done);

            memcpy(msg + msg_len, arg->u_i->buf, arg->u_i->buf_len);
            msg_len += arg->u_i->buf_len;

            memcpy(msg + msg_len, PROGRESS_MID, strlen(PROGRESS_MID));
            msg_len += strlen(PROGRESS_MID);

            memcpy(msg + msg_len, num, strlen(num));
            msg_len += strlen(num);

            progress[i].emitted = 1;
            log_info("Progress back is %s\n", msg);

            msg_back(msg, msg_len, arg->u_i);
        }
    }
}

/* 
 * @brief message final result back
 * @param arg, parameter passed from outside
 * @param msg_back, callback function to send UDP msg to host
 * @succeeded, whether succeeded or failed
 * */
void result_back(struct cmd_arg *arg, udp_msg_back msg_back, int succeeded)
{
    char *msg = NULL;
    int msg_len;
    
    if (succeeded) {
        msg_len = arg->u_i->buf_len + strlen(SUCCEEDED_SUFIX);
        msg = (char*) malloc(msg_len + 1);
        memcpy(msg, arg->u_i->buf, arg->u_i->buf_len);
        memcpy(msg + arg->u_i->buf_len, SUCCEEDED_SUFIX, strlen(SUCCEEDED_SUFIX));
    } else {
        msg_len = arg->u_i->buf_len + strlen(FAILED_SUFIX);
        msg = (char*) malloc(msg_len + 1);
        memcpy(msg, arg->u_i->buf, arg->u_i->buf_len);
        memcpy(msg + arg->u_i->buf_len, FAILED_SUFIX, strlen(FAILED_SUFIX));
    }
    
    if (msg) {
        msg[msg_len] = '\0';
        log_info("Message Back Final Result %s\n", msg);
    }

    msg_back(msg, msg_len, arg->u_i);

    if (msg)
        free(msg);
}
