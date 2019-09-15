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

#ifndef UDP_THREAD_WIDGET_H
#define UDP_THREAD_WIDGET_H

#include <QWidget>
#include "flash_burning_status.h"

class QHBoxLayout;
class QPushButton;
class QTextEdit;
class QLabel;
class QLineEdit;
class QVBoxLayout;

class UdpThread;
class Config;
class FlashTableWidget;

class UdpThreadWidget : public QWidget
{
    Q_OBJECT
public:
    UdpThreadWidget(QWidget *parent = 0);
    ~UdpThreadWidget();

private slots:
    void update_table_widget(const QString &string);
    void send_burn_msg();
    void send_stop_msg();
    void reset();

private:
    bool form_flash_burning_status(const QString &string,
                                   int &index, FlashBurningStatus &f_s);

private:
    UdpThread *udp_thread;
    Config *config;
    QHBoxLayout *hlay;
    QVBoxLayout *vlay;
    QLabel *port_label;
    QLineEdit *port_ledit;
    QPushButton *burn_button;
    QPushButton *stop_button;
    QPushButton *reset_button;
    FlashTableWidget *flash_widget;
};

#endif
