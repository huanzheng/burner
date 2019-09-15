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

#ifndef MSG_BACK_UTILS_H
#define MSG_BACK_UTILS_H
#include "udp_s.h"

/* NOT INTEND FOR THREAD SAFE */

/* NOTE:
 * The progress sent back should be like cmd received plus _p_XX, 
 * say CMD_p_XX, E.g. burn_40_p_99
 * The succeed/fail sent back should be cmd received plus _s/_f, 
 * say CMD_XX_s/CMD_XX_f, E.g. burn_40_s/burn_40_f
 * */

/* Limit the number of progess msgs to be sent back */
#define PROGRESS_STEPS 10

/* 
 * @brief Initialize the prgres array
 * @param total, Image size
 * @param steps, Number of progress msgs, 
 *   !!!!! PLEASE PASS in PROGRESS_STEPS for efficiency
 * */
void progress_helper_init(int total, int steps);

/* 
 * NOTE: message back size, not pecentage, to avoid calculation on board
 * pecentage need to be calculated out on host machine
 * @brief function to send progress UDP message back
 * @param done, size already burned
 * @param arg, parameter passed from outside
 * @param msg_back, callback function to send UDP msg to host
 * */
void progress_back(int done, struct cmd_arg *arg, udp_msg_back msg_back);

/* 
 * @brief message final result back
 * @param arg, parameter passed from outside
 * @param msg_back, callback function to send UDP msg to host
 * @succeeded, whether succeeded or failed
 * */
void result_back(struct cmd_arg *arg, udp_msg_back msg_back, int succeeded);

#endif
