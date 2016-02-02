/*
 *   Copyright (C) 2010 - 2016 by Ivan Cukic <ivan.cukic@kde.org>
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

#ifndef FEATURES_H
#define FEATURES_H

// Qt
#include <QObject>
#include <QString>
#include <QDBusVariant>

// Utils
#include <utils/d_ptr.h>

// Local
#include "Module.h"


/**
 * Features object provides one interface for clients
 * to access other objects' features
 */
class Features : public Module {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.ActivityManager.Features")

public:
    Features(QObject *parent = Q_NULLPTR);
    virtual ~Features();

public Q_SLOTS:
    /**
     * Is the feature backend available?
     */
    bool IsFeatureOperational(const QString &feature) const;

    QStringList ListFeatures(const QString &module) const;

    QDBusVariant GetValue(const QString &property) const;

    void SetValue(const QString &property, const QDBusVariant &value);

private:
    D_PTR;
};

#endif // FEATURES_H
