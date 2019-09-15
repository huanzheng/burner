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

#include "flash_burning_status.h"

FlashBurningStatus::FlashBurningStatus()
{
    b_finished = false;
    b_started = false;
    b_succeeded = false;
    prgres = 0;
    b_switch = false;
}

FlashBurningStatus::FlashBurningStatus(const FlashBurningStatus &s)
{
    b_finished = s.b_finished;
    b_started = s.b_started;
    b_succeeded = s.b_succeeded;
    prgres = s.prgres;
    /* Do Not Copy b_switch */
}

FlashBurningStatus & FlashBurningStatus::operator =(const FlashBurningStatus &s)
{
    b_finished = s.b_finished;
    b_started = s.b_started;
    b_succeeded = s.b_succeeded;
    prgres = s.prgres;
    /* Do Not Copy b_switch */
    return *this;
}

void FlashBurningStatus::set_started()
{
    b_started = true;
    b_finished = false;
    prgres = 0;
}

void FlashBurningStatus::reset()
{
    b_finished = false;
    b_started = false;
    b_succeeded = false;
    prgres = 0;
    b_switch = false;
}

void FlashBurningStatus::set_result(bool succeeded)
{
    b_started = true;
    b_finished = true;
    b_succeeded = succeeded;
    if (b_succeeded)
        prgres = 100;
    else
        prgres = 0;
}

void FlashBurningStatus::set_progress(int p)
{
    b_started = true;
    prgres = p;
}

bool FlashBurningStatus::finished()
{
    return b_finished;
}

bool FlashBurningStatus::result()
{
    return b_succeeded;
}

bool FlashBurningStatus::started()
{
    return b_started;
}

int FlashBurningStatus::progress()
{
    return prgres;
}
