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

#include <QHostAddress>
#include <QFile>
#include <QDataStream>
#include "tcp_file_transfer_client.h"

#define MAXLINE 4096
#define PROGRESS_STEPS 20

QString SockError[] = {
    QString("ConnectionRefusedError"),              /* 0 */
    QString("RemoteHostClosedError"),
    QString("HostNotFoundError"),
    QString("SocketAccessError"),
    QString("SocketResourceError"),
    QString("SocketTimeoutError"),
    QString("DatagramTooLargeError"),
    QString("NetworkError"),
    QString("AddressInUseError"),
    QString("SocketAddressNotAvailableError"),
    QString("UnsupportedSocketOperationError"),
    QString("UnfinishedSocketOperationError"),
    QString("ProxyAuthenticationRequiredError"),
    QString("SslHandshakeFailedError"),
    QString("ProxyConnectionRefusedError"),
    QString("ProxyConnectionClosedError"),
    QString("ProxyConnectionTimeoutError"),
    QString("ProxyNotFoundError"),
    QString("ProxyProtocolError"),
    QString("UnknownSocketError")                   /* 19 */
};

TcpFileTransferClient::TcpFileTransferClient(const QString &ip, const QString &port, const QString &file_p) : QObject(NULL),
                            server_ip(ip), port_num(port), file_path(file_p)
{
    qDebug() << "TcpFileTransferClient created with IP " << server_ip \
            << " port " << port_num << " file " << file_path;
    QObject::connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)),
                     this, SLOT(emit_socket_error(QAbstractSocket::SocketError)));
    QObject::connect(&socket, SIGNAL(connected()),
                     this, SLOT(begin_transfer()));
    QObject::connect(&socket, SIGNAL(disconnected()),
                     this, SLOT(disconnected()));
    socket.setReadBufferSize(8192);
    file = NULL;
    stop_flag = false;
}

TcpFileTransferClient::~TcpFileTransferClient()
{
    if (file != NULL) {
        delete file;
    }
    socket.close();
}

void TcpFileTransferClient::begin_transfer()
{
    qDebug() << "Connected to " << server_ip << " " \
                                << port_num << " " \
                " start to transfer file " << file_path;

    /* First check file status */
    if (file != NULL) {
        delete file;
        file = NULL;
    }

    file = new QFile(file_path);
    file->open(QIODevice::ReadOnly);

    file_size = file->size();
    qDebug() << "Begin transfer, file " << file_path << " size is " << file_size;

    QDataStream f_in(file);
    QDataStream sock_out(&socket);
    char buf[MAXLINE];
    int size_read = 0, size_write = 0, total_write = 0;

    while (1) {
        if (stop_flag)
            break;

        size_read = f_in.readRawData(buf, MAXLINE);
        //qDebug() << "Read " << size_read;

        if (size_read < 0) {
            qDebug() << "Read Error";
            break;
        }

        if (size_read == 0) {
            qDebug() << "End of file";
            break;
        }

        size_write = sock_out.writeRawData(buf, size_read);
        //qDebug() << "Wrote " << size_write;
        total_write += size_write;
        //socket.flush();
        socket.waitForBytesWritten(2000);                 /* Wait until board received data */
        emit_progress_signal(file_size, total_write, PROGRESS_STEPS);

        if (size_read != size_write) {
            qDebug() << "!!!!!!Possbile problem";
        }
    }
    internal_stop();
}

void TcpFileTransferClient::stop()
{
    stop_flag = TRUE;
}

void TcpFileTransferClient::internal_stop()
{
    qDebug() << "disconnect socket";
    if (file)
        file->close();
    socket.disconnectFromHost(); /* use disconnectFromHost to avoid losing data */
}

void TcpFileTransferClient::start()
{
    QHostAddress address(server_ip);
    socket.connectToHost(address, port_num.toUShort(), QIODevice::WriteOnly);
    stop_flag = false;
}

void TcpFileTransferClient::emit_socket_error(QAbstractSocket::SocketError socketError)
{
    internal_stop();
    if (socketError == QAbstractSocket::UnknownSocketError) {
        emit(error(SockError[19]));
    } else {
        emit(error(SockError[(int)(socketError)]));
    }
}

void TcpFileTransferClient::emit_progress_signal(int f_size, int written, int steps)
{
    Q_UNUSED(steps); /* now unused*/
    //qDebug() << "Emit progress " << ((double)(written)) / ((double)(f_size));
    emit progress(((double)(written)) / ((double)(f_size)));
}

void TcpFileTransferClient::disconnected()
{
    if (file)
        file->close();
    socket.close();
    emit finished();
}
