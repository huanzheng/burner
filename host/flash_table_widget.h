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

#ifndef FLASH_TABLE_WIDGET_H
#define FLASH_TABLE_WIDGET_H

#include <QWidget>
#include <QVector>
#include "flash_burning_status.h"

class FlashTableModel;
class FlashTableDelegate;
class FlashTableView;
class QVBoxLayout;

class FlashTableWidget : public QWidget
{
    Q_OBJECT

public:
    FlashTableWidget(int row, int column, QWidget *parent = 0);
    ~FlashTableWidget();
    void set_flash_status(int index, FlashBurningStatus &f);
    QVector<int> get_selected() const;

private:
    FlashTableModel *model;
    FlashTableDelegate *delegate;
    FlashTableView *view;
    QVBoxLayout *vlay;
};
#endif
