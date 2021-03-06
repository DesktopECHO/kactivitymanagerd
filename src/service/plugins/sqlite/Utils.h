/*
 *   Copyright (C) 2014 Ivan Cukic <ivan.cukic(at)kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2,
 *   or (at your option) any later version, as published by the Free
 *   Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PLUGINS_SQLITE_DATABASE_UTILS_H
#define PLUGINS_SQLITE_DATABASE_UTILS_H

#include <QSqlQuery>
#include <QSqlError>
#include <common/database/schema/ResourcesDatabaseSchema.h>
#include <memory>

#include "DebugResources.h"

namespace Utils {

    static unsigned int errorCount = 0;

    inline bool prepare(Common::Database &database,
                        QSqlQuery &query,
                        const QString &queryString)
    {
        Q_UNUSED(database);

        return query.prepare(queryString);
    }

    inline bool prepare(Common::Database &database,
                        std::unique_ptr<QSqlQuery> &query,
                        const QString &queryString)
    {
        if (query) {
            return true;
        }

        query.reset(new QSqlQuery(database.createQuery()));

        return prepare(database, *query, queryString);
    }

    enum ErrorHandling {
        IgnoreError,
        FailOnError
    };

    inline bool exec(Common::Database &database, ErrorHandling eh, QSqlQuery &query)
    {
        bool success = query.exec();

        if (eh == FailOnError) {
            if ((!success) && (errorCount++ < 2)) {
                qCWarning(KAMD_LOG_RESOURCES) << query.lastQuery();
                qCWarning(KAMD_LOG_RESOURCES) << query.lastError();
            }
            Q_ASSERT_X(success, "Uils::exec", qPrintable(QStringLiteral("Query failed:") + query.lastError().text()));

            if (!success) {
                database.reportError(query.lastError());
            }
        }

        return success;
    }

    template <typename T1, typename T2, typename... Ts>
    inline bool exec(Common::Database &database, ErrorHandling eh, QSqlQuery &query,
                     const T1 &variable, const T2 &value, Ts... ts)
    {
        query.bindValue(variable, value);

        return exec(database, eh, query, ts...);
    }

} // namespace Utils


#endif /* !PLUGINS_SQLITE_DATABASE_UTILS_H */
