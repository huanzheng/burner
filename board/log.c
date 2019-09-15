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
#include <stdarg.h>
#include "log.h"

static int log_enabled = 1;

void log_init(int enable) 
{
    log_enabled = enable;
}

void log_info(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    if (log_enabled) {
        vfprintf(stderr, fmt, ap);
    }
    va_end(ap);
    return;
}
