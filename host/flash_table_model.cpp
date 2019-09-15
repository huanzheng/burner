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

#include <QtGui>
#include "flash_table_model.h"
#include "flash_burning_status.h"

#include <stdlib.h>

FlashTableModel::FlashTableModel(int rows, int columns, QObject *parent)
     : QAbstractTableModel(parent)
{
    row_c = rows;
    column_c = columns;

    int total = row_c * column_c;
    int i;

    for (i = 0; i < total; i++) {
        FlashBurningStatus *f_s = new FlashBurningStatus();
        flashes.append(f_s);
    }
    qDebug() << "Flash status number is " << flashes.size();
}

FlashTableModel::~FlashTableModel()
{
    int total = row_c * column_c;
    int i;

    for (i = 0; i < total; i++) {
        delete flashes.value(i);
    }
    flashes.clear();
}

int FlashTableModel::rowCount(const QModelIndex & ) const
{
    return row_c;
}

int FlashTableModel::columnCount(const QModelIndex & ) const
{
    return column_c;
}

QVariant FlashTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int ii;
    FlashBurningStatus *f_s;

    map_row_column_to_index(index.row(), index.column(), ii);

    f_s = flashes.value(ii);

    if (role == FlashIconRole) {
        if (!f_s->started()) {
            return QVariant(QString(NOTSTARTED));
        } else if (f_s->started() && f_s->progress() != 0 && !f_s->finished()) {
            if (f_s->b_switch == true) {
                f_s->b_switch = false;
                return QVariant(QString(STARTED1));
            } else if (f_s->b_switch == false) {
                f_s->b_switch = true;
                return QVariant(QString(STARTED2));
            }
        } else if (f_s->finished()) {
            if (f_s->result()) {
                return QVariant(QString(SUCCEEDED));
            } else {
                return QVariant(QString(FAILED));
            }
        }

    } else if (role == FlashPrgRole) {
        return QVariant(f_s->progress());
    }

    return QVariant();
}

QVariant FlashTableModel::headerData(int ,Qt::Orientation ,int role) const
{
    if (role == Qt::SizeHintRole)
        return QSize(1, 1);
    return QVariant();
}

/* index should start from 0 */
bool FlashTableModel::map_index_to_row_column(int index, int &row, int &column) const
{
    if (index >= row_c * column_c)
        return false;

    row = (index + column_c)/column_c - 1;
    column = (index % column_c);
    return true;
}

void FlashTableModel::map_row_column_to_index(int row, int column, int &index) const
{
    index = row * column_c + column;
}

void FlashTableModel::set_flash_status(int index, FlashBurningStatus &f)
{
    int row, column;
    if (map_index_to_row_column(index, row, column)) {
        qDebug() << "FlashTableModel::set_flash_status " << index;
        *(flashes.value(index)) = f;
        QModelIndex index = createIndex(row, column);
        emit dataChanged(index, index);
    }
}
