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

#ifndef FLASH_TABLE_MODEL_H
#define FLASH_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QVector>

#include "flash_burning_status.h"

enum FlashDataModelRole {
    FlashIconRole = Qt::UserRole + 1,
    FlashPrgRole = Qt::UserRole + 2,
};

#define NOTSTARTED "notstarted"
#define STARTED1 "started1"
#define STARTED2 "started2"
#define FAILED "failed"
#define SUCCEEDED "succeeded"

class FlashTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    FlashTableModel(int rows, int columns, QObject *parent = 0);
    ~FlashTableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void set_flash_status(int index, FlashBurningStatus &f);

    bool map_index_to_row_column(int index, int &row, int &column) const;
    void map_row_column_to_index(int row, int column, int &index) const;

private:
    QVector<FlashBurningStatus *> flashes;
    int row_c;
    int column_c;
};
#endif
