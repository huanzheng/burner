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

#include <QUdpSocket>
#include "udp_thread.h"

UdpThread::UdpThread(const QString &ip, const QString &port)
{
    udp_sock_r = NULL;
    server_ip = ip;
    port_num =  port.toUShort();
}

UdpThread::~UdpThread()
{
    delete udp_sock_r;
    udp_sock_r = NULL;
}

void UdpThread::process_msg()
{
    while (udp_sock_r->hasPendingDatagrams()) {
             QByteArray datagram;
             datagram.resize(udp_sock_r->pendingDatagramSize());
             udp_sock_r->readDatagram(datagram.data(), datagram.size());
             qDebug() << QString(datagram);
             emit get_msg(QString(datagram));
         }
}

void UdpThread::run()
{
    if (udp_sock_r == NULL) {
        udp_sock_r = new QUdpSocket();
        if (!(udp_sock_r->bind(port_num - 2, QUdpSocket::ShareAddress))) {
            qDebug() << "Bind Failed";
        }
        QObject::connect(udp_sock_r, SIGNAL(readyRead()),
                this, SLOT(process_msg()));
    }
    exec();
}

void UdpThread::send_msg(const QByteArray &cmd)
{
    QByteArray datagram = cmd;
    udp_sock_r->writeDatagram(datagram.data(), datagram.size(),
                              QHostAddress(server_ip), port_num);
}
