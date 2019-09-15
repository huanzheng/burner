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

#include "config.h"
#include <QDir>
#include <QSettings>

#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_TCP_PORT "51717"
#define DEFAULT_UDP_PORT "57171"
#define DEFAULT_IMAGE "/zImage"

#define IP_KEY "server_ip"
#define TCP_PORT_KEY "tcp_port"
#define UDP_PORT_KEY "udp_port"
#define IMAGE_KEY "image_path"

Config* Config::instns = NULL;

Config* Config::instance()
{
    if (instns == NULL) {
        instns = new Config();
    }
    return instns;
}

Config::Config()
{
    settings = new QSettings("SNDA", "Burn");
    s_ip = (settings->value(IP_KEY)).toString();
    t_port = (settings->value(TCP_PORT_KEY)).toString();
    u_port = (settings->value(UDP_PORT_KEY)).toString();
    f_path = (settings->value(IMAGE_KEY)).toString();

    if (s_ip == "")
        s_ip = DEFAULT_IP;

    if (t_port == "")
        t_port = DEFAULT_TCP_PORT;

    if (u_port == "")
        u_port = DEFAULT_UDP_PORT;

    if(f_path == "")
        f_path = QDir::homePath() + QString(DEFAULT_IMAGE);
}

Config::~Config()
{
    settings->sync();
    delete settings;
}

void Config::save(const QString &server_ip, const QString &tcp_port,
                  const QString &image_path, const QString &udp_port)
{
    bool need_to_sync = false;

    if (s_ip != server_ip) {
        s_ip = server_ip;
        settings->setValue(IP_KEY, server_ip);
        need_to_sync = true;
    }

    if (t_port != tcp_port) {
        t_port = tcp_port;
        settings->setValue(TCP_PORT_KEY, tcp_port);
        need_to_sync = true;
    }

    if (f_path != image_path) {
        f_path = image_path;
        settings->setValue(IMAGE_KEY, image_path);
        need_to_sync = true;
    }

    if (u_port != udp_port) {
        u_port = udp_port;
        settings->setValue(UDP_PORT_KEY, udp_port);
        need_to_sync = true;
    }

    if (need_to_sync) {
        settings->sync();
    }
}

