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

#include "tcp_file_transfer_client.h"
#include "tcp_file_transfer_client_thread.h"

TcpClientThread::TcpClientThread(const QString &ip, const QString &port, const QString &file)
{
    /* everything here belongs to main thread, so let's do not create tcp client here */
    server_ip = ip;
    port_num = port;
    file_path = file;
    stop_flag = false;
}

TcpClientThread::~TcpClientThread()
{
    delete tcp_client;
}

void TcpClientThread::escalate_error(const QString &error_str)
{
    qDebug() << "Got error " << error_str << " from tcp client";
    if (isRunning())
        quit();
    emit error(error_str);
}

void TcpClientThread::escalate_progress(const double p)
{
    if (!stop_flag)
        emit progress(p);
    else {
        tcp_client->stop();
        stop_flag = false;
    }
}

void TcpClientThread::handle_transfer_finished()
{
    qDebug() << "TcpClient Finished Transfer";

    if (isRunning())
        quit();
}

void TcpClientThread::run()
{
    tcp_client = new TcpFileTransferClient(server_ip, port_num, file_path);
    QObject::connect(tcp_client, SIGNAL(error(const QString &)),
                     this, SLOT(escalate_error(const QString &)));
    QObject::connect(tcp_client, SIGNAL(progress(double)),
                     this, SLOT(escalate_progress(double)));
    QObject::connect(tcp_client, SIGNAL(finished()),
                     this, SLOT(handle_transfer_finished()));
    QObject::connect(this, SIGNAL(finished()),
                     this, SLOT(thread_finished()));
    tcp_client->start();
    exec();
}

void TcpClientThread::thread_finished()
{
    if (tcp_client)
        delete tcp_client;
    tcp_client = NULL;
}

