/*
 *   Copyright (C) 2012 - 2016 by Ivan Cukic <ivan.cukic@kde.org>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2 of
 *   the License or (at your option) version 3 or any later version
 *   accepted by the membership of KDE e.V. (or its successor approved
 *   by the membership of KDE e.V.), which shall act as a proxy
 *   defined in Section 14 of version 3 of the license.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Self
#include "KSMServer.h"
#include "KSMServer_p.h"

// Qt
#include <QDBusConnection>
#include <QDBusServiceWatcher>
#include <QDBusInterface>
#include <QDBusConnectionInterface>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>

// KDE
#include <kdbusconnectionpool.h>

// Utils
#include <utils/d_ptr_implementation.h>

// Local
#include <Debug.h>

#define KWIN_SERVICE QStringLiteral("org.kde.KWin")

KSMServer::Private::Private(KSMServer *parent)
    : serviceWatcher(new QDBusServiceWatcher(this))
    , kwin(Q_NULLPTR)
    , processing(false)
    , q(parent)
{
    serviceWatcher->setConnection(KDBusConnectionPool::threadConnection());
    serviceWatcher->addWatchedService(KWIN_SERVICE);

    connect(serviceWatcher.get(), &QDBusServiceWatcher::serviceOwnerChanged,
            this, &Private::serviceOwnerChanged);

    serviceOwnerChanged(KWIN_SERVICE, QString(), QString());
}

void KSMServer::Private::serviceOwnerChanged(const QString &service,
                                             const QString &oldOwner,
                                             const QString &newOwner)
{
    Q_UNUSED(oldOwner);
    Q_UNUSED(newOwner);

    if (service == KWIN_SERVICE) {
        // Delete the old object, just in case
        delete kwin;
        kwin = Q_NULLPTR;

        if (KDBusConnectionPool::threadConnection().interface()->isServiceRegistered(KWIN_SERVICE)) {
            // Creating the new dbus interface
            // TODO: in multi-head environment there are multiple kwin instances
            //       running and they will export different dbus name on different
            //       root window. We have no support for that currently.
            //       In future, the session management for Wayland may also need to be
            //       reimplemented in some way.
            kwin = new QDBusInterface(KWIN_SERVICE, QStringLiteral("/KWin"), QStringLiteral("org.kde.KWin"));

            // If the service is valid, initialize it
            // otherwise delete the object
            if (kwin->isValid()) {
                kwin->setParent(this);

            } else {
                delete kwin;
                kwin = Q_NULLPTR;
            }
        }
    }
}

KSMServer::KSMServer(QObject *parent)
    : QObject(parent)
    , d(this)
{
}

KSMServer::~KSMServer()
{
}

void KSMServer::startActivitySession(const QString &activity)
{
    d->processLater(activity, true);
}

void KSMServer::stopActivitySession(const QString &activity)
{
    d->processLater(activity, false);
}

void KSMServer::Private::processLater(const QString &activity, bool start)
{
    if (kwin) {
        for (const auto &item: queue) {
            if (item.first == activity) {
                return;
            }
        }

        queue << qMakePair(activity, start);

        if (!processing) {
            processing = true;
            QMetaObject::invokeMethod(this, "process", Qt::QueuedConnection);
        }
    } else {
        // We don't have kwin. No way to invoke the session stuff
        emit subSessionSendEvent(start ? KSMServer::Started
                                       : KSMServer::Stopped);
    }
}

void KSMServer::Private::process()
{
    // If the queue is empty, we have nothing more to do
    if (queue.isEmpty()) {
        processing = false;
        return;
    }

    const auto item = queue.takeFirst();
    processingActivity = item.first;

    makeRunning(item.second);

    // Calling process again for the rest of the list
    QMetaObject::invokeMethod(this, "process", Qt::QueuedConnection);
}

void KSMServer::Private::makeRunning(bool value)
{
    if (!kwin) {
        subSessionSendEvent(value ? KSMServer::Started : KSMServer::Stopped);
        return;
    }

    const auto call = kwin->asyncCall(
        value ? QLatin1String("startActivity") : QLatin1String("stopActivity"),
        processingActivity);

    const auto watcher = new QDBusPendingCallWatcher(call, this);

    QObject::connect(
        watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
        this,
        value
            ? SLOT(startCallFinished(QDBusPendingCallWatcher *))
            : SLOT(stopCallFinished(QDBusPendingCallWatcher *)));
}

void KSMServer::Private::startCallFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<bool> reply = *call;

    if (reply.isError()) {
        emit subSessionSendEvent(KSMServer::Started);

    } else {
        // If we got false, it means something is going on with ksmserver
        // and it didn't start our activity
        const auto retval = reply.argumentAt<0>();

        if (!retval) {
            subSessionSendEvent(KSMServer::Stopped);
        } else {
            subSessionSendEvent(KSMServer::Started);
        }
    }

    call->deleteLater();
}

void KSMServer::Private::stopCallFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<bool> reply = *call;

    if (reply.isError()) {
        subSessionSendEvent(KSMServer::Stopped);

    } else {
        // If we got false, it means something is going on with ksmserver
        // and it didn't stop our activity
        const auto retval = reply.argumentAt<0>();

        if (!retval) {
            subSessionSendEvent(KSMServer::FailedToStop);
        } else {
            subSessionSendEvent(KSMServer::Stopped);
        }
    }

    call->deleteLater();
}

void KSMServer::Private::subSessionSendEvent(int event)
{
    if (processingActivity.isEmpty()) {
        return;
    }

    emit q->activitySessionStateChanged(processingActivity, event);

    processingActivity.clear();
}
