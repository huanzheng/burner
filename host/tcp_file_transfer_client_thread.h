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

#ifndef TCP_FILE_TRANSFER_CLIENT_THREAD_H
#define TCP_FILE_TRANSFER_CLIENT_THREAD_H

#include <QThread>

class TcpFileTransferClient;

class TcpClientThread : public QThread
{
    Q_OBJECT
public:
    TcpClientThread(const QString &ip, const QString &port, const QString &file);
    ~TcpClientThread();

    void set_ip(const QString &ip) { server_ip = ip;}
    void set_port(const QString &port) { port_num = port;}
    void set_file_path(const QString &file) { file_path = file; }

    void set_stop(bool flag) {stop_flag = flag;}

protected:
    void run();

private slots:
    void escalate_error(const QString &error_str);
    void escalate_progress(const double p);
    void handle_transfer_finished();
    void thread_finished();

signals:
    void progress(const double p);
    void error(const QString &error_str);

private:
    TcpFileTransferClient *tcp_client;
    QString server_ip;
    QString port_num;
    QString file_path;
    bool stop_flag;
};

#endif
