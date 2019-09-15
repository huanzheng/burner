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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "udp_s.h"
#include "log.h"
#include "io.h"
#include "msg_back_utils.h"

/* NOTE:
 * A valid burn command should be like burn_XX, E.g. burn_1 burn_2 ... burn_48 
 * The progress sent back should be like burn_XX_p_XX, E.g. burn_40_p_99
 * The succeed/fail sent back should be like burn_XX_s/burn_XX_f, E.g. burn_40_s/burn_40_f
 * */

#define UNDERSCORE '_'
#define BURN_NUM_MAX_LENGTH 2
#define BURN_PREFIX_LENGTH 5
#define MAXLINE 4096

/* 
 * @brief, Check whether the command is valid or not
 * @param cmd, the burn cmd got from host
 * @param cmd_len, the length of the cmd
 * @param num, return the XX of burn_XX if it is valid cmd
 * @return, 1 if it is valid, 0 if it is not
 * */
static int valid_burn(char *cmd, int cmd_len, char *num)
{
    int i, j;
    if (cmd_len < BURN_PREFIX_LENGTH + 1 ||
        cmd_len > BURN_PREFIX_LENGTH + BURN_NUM_MAX_LENGTH)
        return 0;

    if (cmd[4] != UNDERSCORE)
        return 0;
    
    i = BURN_PREFIX_LENGTH;
    j = 0;
    while (i < cmd_len) {
        if (cmd[i] >= '0' && cmd[i] <= '9') {
            num[j] = cmd[i];
        } else
            return 0;
        i++;
        j++;
    }
    return 1;
}


static int zbt_fake_burn(char *from, char *dst, struct cmd_arg *arg, udp_msg_back msg_back)
{
    int from_fd, dst_fd;
    char buf[MAXLINE];
    ssize_t size_read;
    ssize_t size_write;
    int ret = 1;
    ssize_t total_written = 0;

    from_fd = open(from, O_RDONLY);
    if (from_fd < 0) {
        fprintf(stderr, "File %s open failed\n", from);
        return 0;
    }

    if ((dst_fd = open(dst, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) < 0) {
        fprintf(stderr, "open file %s failed\n", dst);
        close(from_fd);
        return 0;
    }

    while (1) {
        size_read = readn(from_fd, buf, MAXLINE);

        if (size_read == 0) {
            log_info("Read %s finished\n", from);
            ret = 1;
            break;
        } else if (size_read < 0) {
            fprintf(stderr, "Read %s failed\n", from);
            ret = 0;
            break;
        }

        size_write = writen(dst_fd, buf, size_read);
        total_written += size_write;
        progress_back(total_written, arg, msg_back);
        if (size_write != size_read) {
            fprintf(stderr, "wrote failed\n");
            ret = 0;
            //break;
        }
    }

    close(from_fd);
    close(dst_fd);

    return ret;
}

/* 
 * @brief, do the real work of burn
 * @param arg, parameter passed from outside
 * @param msg_back, callback function to send UDP msg to host
 * @param num, burn number, it is a string with '\0' in the end
 * @return, 1 if burn succeeded, 0 if burn failed
 * */
static int real_burn(struct cmd_arg *arg, udp_msg_back msg_back, char *num)
{
    char *image_path = arg->image_path;
    struct stat image_stat;

    if (!stat(image_path, &image_stat)) {
        int file_size = image_stat.st_size;
        char dst_file[256];
        bzero(dst_file, 256);
        
        strcpy(dst_file, image_path);
        strcat(dst_file, num);

        /* Initilize progress helper */
        progress_helper_init(file_size, PROGRESS_STEPS);
        
        log_info("Burn From %s to %s, file size is %d\n", image_path, dst_file, file_size);

        /* Replace with FLASH BURN proceedure */
        return zbt_fake_burn(image_path, dst_file, arg, msg_back);
    } else
        return 0;
}


void burn(struct cmd_arg *arg, udp_msg_back msg_back)
{
    struct udp_info *u_i = arg->u_i;
    char burn_num[BURN_NUM_MAX_LENGTH + 1];
    log_info("Image path is %s\n", arg->image_path);

    bzero(burn_num, BURN_NUM_MAX_LENGTH + 1);
    if (valid_burn(u_i->buf, u_i->buf_len, burn_num)) {
        if (real_burn(arg, msg_back, burn_num)) {
            result_back(arg, msg_back, 1);
        } else {
            result_back(arg, msg_back, 0);
        }
    } else {
        log_info("Not a valid burn command\n");
    }
    return;
}

