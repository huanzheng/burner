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
#include <QVBoxLayout>

#include "flash_table_model.h"
#include "flash_table_delegate.h"
#include "flash_table_widget.h"
#include "flash_table_view.h"

FlashTableWidget::FlashTableWidget(int row, int column, QWidget *parent) : QWidget(parent)
{
    model = new FlashTableModel(row, column, this);
    delegate = new FlashTableDelegate(this);
    vlay = new QVBoxLayout(this);

    view = new FlashTableView;
    view->setShowGrid(false);
    view->horizontalHeader()->hide();
    view->verticalHeader()->hide();
    view->horizontalHeader()->setMinimumSectionSize(1);
    view->verticalHeader()->setMinimumSectionSize(1);

    view->setModel(model);
    view->setItemDelegate(delegate);

    view->resizeColumnsToContents();
    view->resizeRowsToContents();
    vlay->addWidget(view);
}

FlashTableWidget::~FlashTableWidget()
{
    delete delegate;
    delete model;
    delete view;
    delete vlay;
}

void FlashTableWidget::set_flash_status(int index, FlashBurningStatus &f)
{
    return model->set_flash_status(index, f);
}

QVector<int> FlashTableWidget::get_selected() const
{
    QList<QModelIndex> selected = view->selected_indexes();
    QVector<int> result;

    for (QList<QModelIndex>::const_iterator i = selected.begin(); i != selected.end(); i++) {
        int index;
        model->map_row_column_to_index(i->row(), i->column(), index);
        result.append(index);
    }
    return result;
}
