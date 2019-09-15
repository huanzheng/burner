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

#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

#include "tcp_file_transfer_client_widget.h"
#include "tcp_file_transfer_client_thread.h"
#include "config.h"

#define LABELMINIWIDTH 100
#define BUTTONMINIWIDTH 95
#define LABEL_IP "板子网络地址"
#define LABEL_PORT "板子端口号"
#define LABEL_PATH "映像文件"
#define LABEL_PROGRESS "传输进度"


#define BUTTON_CHOOSE "选择文件"
#define BUTTON_TRANSPORT "传输"
#define BUTTON_STOP "停止传输"

TcpFileTransferClientWidget::TcpFileTransferClientWidget(QWidget *parent) : QWidget(parent)
{
    config = Config::instance();

    tcp_thread = new TcpClientThread(config->server_ip(),
                                     config->tcp_port(), config->image_path());
    /* Layout */
    ip_hlay = new QHBoxLayout();
    port_hlay = new QHBoxLayout();
    path_hlay = new QHBoxLayout();
    progress_hlay = new QHBoxLayout();

    vlay = new QVBoxLayout();

    /* IP Edit Stuff */
    ip = new QLabel();
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(ip->sizePolicy().hasHeightForWidth());
    ip->setSizePolicy(sizePolicy);
    ip->setMinimumSize(QSize(LABELMINIWIDTH,0));
    ip->setText(QString::fromUtf8(LABEL_IP));
    ip_hlay->addWidget(ip);

    ip_edit = new QLineEdit();
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(ip_edit->sizePolicy().hasHeightForWidth());
    ip_edit->setSizePolicy(sizePolicy1);
    ip_edit->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    ip_edit->setText(config->server_ip());
    ip_hlay->addWidget(ip_edit);

    stop = new QPushButton();
    stop->setText(QString::fromUtf8(BUTTON_STOP));
    stop->setMinimumSize(QSize(BUTTONMINIWIDTH, 0));
    ip_hlay->addWidget(stop);

    vlay->addLayout(ip_hlay);

    /* Port Edit Stuff */
    port = new QLabel();
    QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(port->sizePolicy().hasHeightForWidth());
    port->setSizePolicy(sizePolicy3);
    port->setMinimumSize(QSize(LABELMINIWIDTH,0));
    port->setText(QString::fromUtf8(LABEL_PORT));
    port_hlay->addWidget(port);

    port_edit = new QLineEdit();
    QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(port_edit->sizePolicy().hasHeightForWidth());
    port_edit->setSizePolicy(sizePolicy4);
    port_edit->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    port_edit->setText(config->tcp_port());
    port_hlay->addWidget(port_edit);

    port_invisible = new QLabel();
    QSizePolicy sizePolicy5(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy5.setHorizontalStretch(0);
    sizePolicy5.setVerticalStretch(0);
    sizePolicy5.setHeightForWidth(port_invisible->sizePolicy().hasHeightForWidth());
    port_invisible->setSizePolicy(sizePolicy5);
    port_invisible->setMinimumSize(QSize(BUTTONMINIWIDTH, 0));
    port_hlay->addWidget(port_invisible);

    vlay->addLayout(port_hlay);

    /* File Choose Stuff */
    path = new QLabel();
    QSizePolicy sizePolicy6(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy6.setHorizontalStretch(0);
    sizePolicy6.setVerticalStretch(0);
    sizePolicy6.setHeightForWidth(path->sizePolicy().hasHeightForWidth());
    path->setSizePolicy(sizePolicy6);
    path->setMinimumSize(QSize(LABELMINIWIDTH,0));
    path->setText(QString::fromUtf8(LABEL_PATH));
    path_hlay->addWidget(path);

    path_edit = new QLineEdit();
    QSizePolicy sizePolicy7(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy7.setHorizontalStretch(0);
    sizePolicy7.setVerticalStretch(0);
    sizePolicy7.setHeightForWidth(path_edit->sizePolicy().hasHeightForWidth());
    path_edit->setSizePolicy(sizePolicy7);
    path_edit->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    path_edit->setText(config->image_path());
    path_hlay->addWidget(path_edit);

    choose = new QPushButton();
    choose->setText(QString::fromUtf8(BUTTON_CHOOSE));
    choose->setMinimumSize(QSize(BUTTONMINIWIDTH, 0));
    path_hlay->addWidget(choose);
    vlay->addLayout(path_hlay);

    /* Progress Stuff */
    prog = new QLabel();
    prog->setMinimumSize(QSize(LABELMINIWIDTH, 0));
    prog->setText(QString::fromUtf8(LABEL_PROGRESS));
    progress_hlay->addWidget(prog);

    progress = new QProgressBar();
    progress->setValue(0);
    progress_hlay->addWidget(progress);

    transport = new QPushButton();
    transport->setMinimumSize(QSize(BUTTONMINIWIDTH, 0));
    transport->setText(QString::fromUtf8(BUTTON_TRANSPORT));
    progress_hlay->addWidget(transport);
    vlay->addLayout(progress_hlay);

    /* Final Layout */
    setLayout(vlay);

    /* Signal handling */
    QObject::connect(choose, SIGNAL(clicked()), this, SLOT(handle_choose_file()));
    QObject::connect(transport, SIGNAL(clicked()), this, SLOT(handle_transport()));
    QObject::connect(stop, SIGNAL(clicked()), this, SLOT(handle_stop()));

    QObject::connect(tcp_thread, SIGNAL(error(const QString &)),
                     this, SLOT(handle_error(const QString &)));
    QObject::connect(tcp_thread, SIGNAL(progress(double)),
                     this, SLOT(handle_progress(double)));

    QObject::connect(tcp_thread, SIGNAL(started()),
                     this, SLOT(thread_started()));
    QObject::connect(tcp_thread, SIGNAL(terminated()),
                     this, SLOT(thread_terminated()));
    QObject::connect(tcp_thread, SIGNAL(finished()),
                     this, SLOT(thread_finished()));
}

TcpFileTransferClientWidget::~TcpFileTransferClientWidget()
{
    delete ip;
    delete port;
    delete path;
    delete prog;
    delete port_invisible;

    delete ip_edit;
    delete port_edit;
    delete path_edit;

    delete choose;
    delete transport;
    delete stop;

    delete progress;

    delete ip_hlay;
    delete port_hlay;
    delete path_hlay;
    delete progress_hlay;

    delete vlay;

    delete tcp_thread;
}

void TcpFileTransferClientWidget::handle_choose_file()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                ("Open Image"), QDir::homePath());
    path_edit->setText(fileName);
}

void TcpFileTransferClientWidget::handle_transport()
{
    progress->setValue(0);

    if (ip_edit->text() == QString("") || port_edit->text() == QString("")) {
        QMessageBox::critical(this, QString("Error"), QString("IP and Port Can't be Empty"));
        return;
    }

    QFile file(path_edit->text());

    if (!file.exists()) {
        qDebug() << "File " << path_edit->text() << " not found";
        QMessageBox::critical(this, QString("Error"), QString("Can't Find File"));
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "File " << path_edit->text() << " open failed";
        QMessageBox::critical(this, QString("Error"), QString("Can't Open File"));
        return;
    } else {
        file.close();
    }

    tcp_thread->set_ip(ip_edit->text());
    tcp_thread->set_port(port_edit->text());
    tcp_thread->set_file_path(path_edit->text());
    config->save(ip_edit->text(), port_edit->text(),
                 path_edit->text(), config->udp_port()); /* Currently UDP Port is not changed */

    transport->setDisabled(TRUE);
    tcp_thread->set_stop(false);

    tcp_thread->start();
}

void TcpFileTransferClientWidget::handle_error(const QString &error_str)
{
    QMessageBox::critical(this, QString("Error"), error_str);
}

void TcpFileTransferClientWidget::handle_progress(double prgres)
{
    //qDebug() << " P " << prgres;
    progress->setValue(prgres * 100);
}

void TcpFileTransferClientWidget::handle_stop()
{
    tcp_thread->set_stop(true);
}

void TcpFileTransferClientWidget::thread_finished()
{
    transport->setDisabled(FALSE);
}

