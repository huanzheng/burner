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

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QByteArray>
#include <QSizePolicy>
#include <QMessageBox>
#include <QtDebug>
#include "udp_thread_widget.h"
#include "udp_thread.h"
#include "config.h"
#include "flash_table_widget.h"

#define MINIWIDTH 100
#define UDP_PORT "UDP端口号"
#define BURN "开始烧写"
#define STOPBURN "停止烧写"
#define RESET "重置"

UdpThreadWidget::UdpThreadWidget(QWidget *parent): QWidget (parent)
{
    config = Config::instance();
    udp_thread = new UdpThread(config->server_ip(), config->udp_port());

    hlay = new QHBoxLayout(this);
    vlay = new QVBoxLayout();

    port_label = new QLabel();
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(port_label->sizePolicy().hasHeightForWidth());
    port_label->setSizePolicy(sizePolicy);
    port_label->setMinimumSize(QSize(MINIWIDTH,0));
    port_label->setText(QString::fromUtf8(UDP_PORT));
    vlay->addWidget(port_label);

    port_ledit = new QLineEdit();
    QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(port_ledit->sizePolicy().hasHeightForWidth());
    port_ledit->setSizePolicy(sizePolicy1);
    port_ledit->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    port_ledit->setMinimumSize(QSize(MINIWIDTH,0));
    vlay->addWidget(port_ledit);
    port_ledit->setText(config->udp_port());

    burn_button = new QPushButton();
    burn_button->setText(QString::fromUtf8(BURN));
    vlay->addWidget(burn_button);

    stop_button = new QPushButton();
    stop_button->setText(QString::fromUtf8(STOPBURN));
    vlay->addWidget(stop_button);

    reset_button = new QPushButton();
    reset_button->setText(QString::fromUtf8(RESET));
    vlay->addWidget(reset_button);

    vlay->addStretch(1);

    flash_widget = new FlashTableWidget(6, 8, this);
    hlay->addWidget(flash_widget);
    hlay->addLayout(vlay);

    QObject::connect(burn_button, SIGNAL(clicked()), this, SLOT(send_burn_msg()));
    QObject::connect(stop_button, SIGNAL(clicked()), this, SLOT(send_stop_msg()));
    QObject::connect(reset_button, SIGNAL(clicked()), this, SLOT(reset()));
    QObject::connect(udp_thread, SIGNAL(get_msg(QString)),
                     this, SLOT(update_table_widget(QString)));

    udp_thread->start();
}

UdpThreadWidget::~UdpThreadWidget()
{
    delete hlay;
    delete vlay;
    delete port_label;
    delete port_ledit;
    delete burn_button;
    delete stop_button;
    delete flash_widget;

    delete udp_thread;
}

void UdpThreadWidget::update_table_widget(const QString &string)
{
    int index;
    FlashBurningStatus fs;

    if (form_flash_burning_status(string, index, fs)) {
        flash_widget->set_flash_status(index, fs);
    }
}

void UdpThreadWidget::send_burn_msg()
{
    QString udp_port = port_ledit->text();
    config->save(config->server_ip(), config->tcp_port(),
                 config->image_path(), udp_port);

    udp_thread->set_ip(config->server_ip());
    udp_thread->set_port(config->udp_port());

    QVector<int> burn_nums = flash_widget->get_selected();

    if (burn_nums.size() == 0) {
        QMessageBox::critical(this, QString("Error"), QString("Select Flash To Burn"));
    }

    for (QVector<int>::iterator i = burn_nums.begin(); i != burn_nums.end(); i++) {
        QByteArray cmd("burn_");
        cmd.append(QString("%1").arg(*i));
        qDebug() << "Send CMD " << cmd;
        udp_thread->send_msg(cmd);
    }
}

void UdpThreadWidget::send_stop_msg()
{
    QString udp_port = port_ledit->text();
    config->save(config->server_ip(), config->tcp_port(),
                 config->image_path(), udp_port);

    udp_thread->set_ip(config->server_ip());
    udp_thread->set_port(config->udp_port());

    udp_thread->send_msg("stop");
}

bool UdpThreadWidget::form_flash_burning_status(const QString &string, int &index, FlashBurningStatus &f_s)
{
    /* Copied from burn.c of board program.
        NOTE:
     32  * A valid burn command should be like burn_XX, E.g. burn_1 burn_2 ... burn_48
     33  * The progress sent back should be like burn_XX_p_XX, E.g. burn_40_p_99
     34  * The succeed/fail sent back should be like burn_XX_s/burn_XX_f, E.g. burn_40_s/burn_40_f
     35  * */

    /* valid msg back should start with burn_ */
    if (!string.startsWith(QString("burn_")))
        return false;

    /* let's get the index, at least it should contain two '_' */
    int i, j;
    i = string.indexOf(QString("_"));
    if (i < 0)
        return false;

    j = string.indexOf(QString("_"), i + 1);
    if (j < 0 || j == (string.length() - 1))
        return false;

    QString index_string = string.mid(i + 1, j - i - 1);
    index = index_string.toInt();

    QString state = string.mid(j + 1, 1);
    qDebug() << "Index is " << index << " State is " << state;

    if (state == QString("s")) {
        f_s.set_result(true);
    } else if (state == QString("f")) {
        f_s.set_result(false);
    } else if (state == QString("p")) {
        /* Try to get progress TODO*/
        f_s.set_progress(50);
    }
    return true;
}

void UdpThreadWidget::reset()
{
    QVector<int> burn_nums = flash_widget->get_selected();

    if (burn_nums.size() == 0) {
        QMessageBox::critical(this, QString("Error"), QString("Select Flash To Burn"));
    }

    for (QVector<int>::iterator i = burn_nums.begin(); i != burn_nums.end(); i++) {
        FlashBurningStatus f;
        flash_widget->set_flash_status(*i, f);
    }
}
