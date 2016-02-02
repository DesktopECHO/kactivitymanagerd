/*
 *   Copyright (C) 2011 - 2016 by Ivan Cukic <ivan.cukic@kde.org>
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

#ifndef PLUGIN_H
#define PLUGIN_H

#include "kactivitymanagerd_plugin_export.h"

// Qt
#include <QObject>
#include <QMetaObject>

// KDE
#include <kpluginfactory.h>
#include <kconfiggroup.h>

// Utils
#include <utils/d_ptr.h>

// Local
#include "Event.h"
#include "Module.h"

#define KAMD_EXPORT_PLUGIN(libname, classname, jsonFile) \
        K_PLUGIN_FACTORY_WITH_JSON(factory, jsonFile, registerPlugin<classname>();)

/**
 *
 */
class KACTIVITYMANAGERD_PLUGIN_EXPORT Plugin : public Module {
    Q_OBJECT

public:
    Plugin(QObject *parent);
    virtual ~Plugin();

    /**
     * Initializes the plugin.
     * @arg modules Activities, Resources and Features manager objects
     * @returns the plugin needs to return whether it has
     *      successfully been initialized
     */
    virtual bool init(QHash<QString, QObject *> &modules) = 0;

    /**
     * Returns the config group for the plugin.
     * In order to use it, you need to set the plugin name.
     */
    KConfigGroup config() const;
    QString name() const;

    /**
     * Convenience meta-method to provide prettier invocation of QMetaObject::invokeMethod
     */
    // template <typename ReturnType>
    // inline static ReturnType retrieve(QObject *object, const char *method,
    //                                   const char *returnTypeName)
    // {
    //     ReturnType result;
    //
    //     QMetaObject::invokeMethod(
    //         object, method, Qt::DirectConnection,
    //         QReturnArgument<ReturnType>(returnTypeName, result));
    //
    //     return result;
    // }

    template <typename ReturnType, typename... Args>
    inline static ReturnType retrieve(QObject *object, const char *method,
                                      const char *returnTypeName,
                                      Args... args)
    {
        ReturnType result;

        QMetaObject::invokeMethod(
            object, method, Qt::DirectConnection,
            QReturnArgument<ReturnType>(returnTypeName, result),
            args...);

        return result;
    }

    /**
     * Convenience meta-method to provide prettier invocation of QMetaObject::invokeMethod
     */
    // template <Qt::ConnectionType connection = Qt::QueuedConnection>
    // inline static void invoke(QObject *object, const char *method,
    //                            const char *returnTypeName)
    // {
    //     Q_UNUSED(returnTypeName);
    //     QMetaObject::invokeMethod(object, method, connection);
    // }

    template <Qt::ConnectionType connection, typename... Args>
    inline static void invoke(QObject *object, const char *method,
                              Args... args)
    {
        QMetaObject::invokeMethod(
            object, method, connection,
            args...);
    }

protected:
    void setName(const QString &name);

private:
    D_PTR;
};

#endif // PLUGIN_H
