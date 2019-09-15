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
#include <QtDebug>

#include "flash_table_delegate.h"
#include "flash_table_model.h"

FlashTableDelegate::FlashTableDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
    if (image_burn.load(":/images/burn.png") && image_ok.load(":/images/ok.png")
        && image_fail.load(":/images/fail.png") && image_burning.load(":/images/burning.png")
        && image_burning1.load(":/images/burning1.png"))
        qDebug() << "Load Image Succeeded";
    else
        qDebug() << "Load Image Failed";
    image_width = image_burn.width();
    image_height = image_burn.height();
}

void FlashTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    if (option.state & QStyle::State_Selected)
        painter->setBrush(option.palette.highlightedText());
    else
        painter->setBrush(QBrush(Qt::black));
#if 0
    qDebug() << "Draw inside " << (QRectF(option.rect.x(), option.rect.y(),
                                          option.rect.width(), option.rect.height()));
#endif
    QImage image_d;
    QString i_s = (index.data(FlashIconRole)).toString();
    int progress = (index.data(FlashPrgRole)).toInt();

    Q_UNUSED(progress); /* avoid warning now */

    if (i_s == QString(NOTSTARTED)) {
        image_d = image_burn;
    } else if (i_s == QString(STARTED1)) {
        image_d = image_burning;
    } else if (i_s == QString(STARTED2)) {
        image_d = image_burning1;
    } else if (i_s == QString(FAILED)) {
        image_d = image_fail;
    } else if (i_s == QString(SUCCEEDED)) {
        image_d = image_ok;
    }

    painter->drawImage(QRectF(option.rect.x(), option.rect.y(),
                              option.rect.width(), option.rect.height()), image_d);
    painter->restore();
}

QSize FlashTableDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
    return QSize(image_width, image_height);
}
