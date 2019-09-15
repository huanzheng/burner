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

#ifndef TCP_FILE_TRANSFER_CLIENT_H
#define TCP_FILE_TRANSFER_CLIENT_H

#include <QtCore>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QFile>

class TcpFileTransferClient : public QObject
{
    Q_OBJECT
public:
    TcpFileTransferClient(const QString &ip, const QString &port, const QString &file);
    ~TcpFileTransferClient();

public slots:
    void start();
    void stop();
    void set_ip(const QString &ip) { server_ip = ip;}
    void set_port(const QString &port) { port_num = port;}
    void set_file_path(const QString &file) { file_path = file; }

private slots:
    void begin_transfer();
    void emit_socket_error(QAbstractSocket::SocketError socketError);
    void emit_progress_signal(int f_size, int written, int steps);
    void disconnected();
    void internal_stop();

signals:
    void progress(const double p);
    void error(const QString &error_str);
    void finished();

private:
    QString server_ip;
    QString port_num;
    QString file_path;
    unsigned long file_size;
    QTcpSocket socket;
    QFile *file;
    bool stop_flag;
};

#endif
