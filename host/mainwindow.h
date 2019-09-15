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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class TcpFileTransferClientWidget;
class UdpThreadWidget;
class QHBoxLayout;
class QVBoxLayout;
class QWidget;
class QFrame;
class QPushButton;
class QTableWidget;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    //Ui::MainWindow *ui;
    TcpFileTransferClientWidget *tcp_widget;
    UdpThreadWidget *udp_widget;

    QVBoxLayout *central_v_layout;

    QWidget *central_widget;
    QFrame *line;
};

#endif // MAINWINDOW_H
