/*
 * Copyright (C) 2015  Vishesh Handa <vhanda@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "console.h"
#include "whatsappjsinterface.h"

extern "C" {
    #include "linenoise.h"
}

#include <QDebug>
#include <QByteArray>

#include <iostream>
#include <stdio.h>

Console::Console(WhatsAppJsInterface* interface, QObject* parent)
    : QObject(parent)
    , m_interface(interface)
{
    connect(m_interface, &WhatsAppJsInterface::chatListChanged, [&]() {
        QVariantList list = m_interface->chatList().toList();

        QTextStream stream(stdout);
        for (int i = 0; i < list.size(); i++) {
            QVariantMap map = list[i].toMap();

            const QString title = map.value("title").toString();
            stream << "[" << i << "] " << title << "\n";
        }
    });
}

void Console::startLoop()
{
    char* line = 0;

    while ((line = ::linenoise("> ")) != 0) {
        QByteArray input(line);
        free(line);

        if (input.isEmpty())
            continue;

        linenoiseHistoryAdd(input.constData());

        if (input == "showContactList") {
            qDebug() << "SHOW CONTACT LIST";
            QMetaObject::invokeMethod(m_interface, "showContactListInvoked", Qt::QueuedConnection);
        }
        else if (input == "hideContactList") {
            qDebug() << "HIDE CONTACT LIST";
            QMetaObject::invokeMethod(m_interface, "hideContactListInvoked", Qt::QueuedConnection);
        }
        else if (input == "chatList") {
            QMetaObject::invokeMethod(m_interface, "populateChatList", Qt::QueuedConnection);
        }
    }
}

