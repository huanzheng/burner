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

#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
class QSettings;

class Config
{
public:
    static Config *instance();
    QString server_ip() {return s_ip;}
    QString tcp_port() {return t_port;}
    QString udp_port() {return u_port;}
    QString image_path() {return f_path;}

    void save(const QString &server_ip, const QString &tcp_port,
              const QString &image_path, const QString &udp_port);

    ~Config();

private:
    static Config *instns;

    Config();
    QSettings *settings;
    QString s_ip;
    QString t_port;
    QString u_port;
    QString f_path;
};

#endif
