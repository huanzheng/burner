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

#ifndef TCP_FILE_TRANSFER_CLIENT_WIDGET_H
#define TCP_FILE_TRANSFER_CLIENT_WIDGET_H

#include <QWidget>
#include <QAbstractSocket>
#include <QThread>
#include <QtDebug>

class QLabel;
class QPushButton;
class QProgressBar;
class QLineEdit;
class QHBoxLayout;
class QVBoxLayout;

class TcpClientThread;
class Config;

class TcpFileTransferClientWidget : public QWidget
{
    Q_OBJECT
public:
    TcpFileTransferClientWidget(QWidget *parent);
    ~TcpFileTransferClientWidget();

private:
    QLabel *ip;
    QLabel *port;
    QLabel *path;
    QLabel *prog;
    //QLabel *ip_invisible;
    QLabel *port_invisible;

    QLineEdit *ip_edit;
    QLineEdit *port_edit;
    QLineEdit *path_edit;

    QPushButton *choose;
    QPushButton *transport;
    QPushButton *stop;

    QProgressBar *progress;

    QHBoxLayout *ip_hlay;
    QHBoxLayout *port_hlay;
    QHBoxLayout *path_hlay;
    QHBoxLayout *progress_hlay;

    QVBoxLayout *vlay;
    TcpClientThread *tcp_thread;
    Config *config;

private slots:
    void handle_choose_file();
    void handle_transport();
    void handle_error(const QString &error_str);
    void handle_progress(double prgres);
    void handle_stop();

    void thread_finished();
    void thread_terminated(){qDebug() << QString("thread terminated");}
    void thread_started(){qDebug() << QString("thread started");}
};
#endif
