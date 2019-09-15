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

#ifndef UDP_THREAD_H
#define UDP_THREAD_H

#include <QThread>
class QUdpSocket;

class UdpThread : public QThread
{
    Q_OBJECT
public:
    UdpThread(const QString &ip, const QString &port);
    ~UdpThread();

public slots:
    void send_msg(const QByteArray &cmd);
    void set_ip(const QString &ip) {server_ip = ip;}
    void set_port(const QString &port) {port_num = port.toUShort();}

protected:
    void run();

private slots:
    void process_msg();

signals:
    void get_msg(const QString &msg);

private:
    QUdpSocket *udp_sock_r;
    QString server_ip;
    quint16 port_num;
};

#endif
