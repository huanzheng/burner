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
#include <QFrame>
#include <QTableWidget>

#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include "udp_thread_widget.h"
#include "tcp_file_transfer_client_widget.h"
#include "tcp_file_transfer_client.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    //ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
    resize(600, 480);

    central_widget = new QWidget(this);
    central_v_layout = new QVBoxLayout(central_widget);

    tcp_widget = new TcpFileTransferClientWidget(this);
    central_v_layout->addWidget(tcp_widget);

    line = new QFrame(central_widget);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    central_v_layout->addWidget(line);

    udp_widget = new UdpThreadWidget(this);
    central_v_layout->addWidget(udp_widget);

    setCentralWidget(central_widget);
}

MainWindow::~MainWindow()
{
   // delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        //ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

