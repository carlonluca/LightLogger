/*
 * Author:  Luca Carlon
 * Company: -
 * Date:    17.05.2023
 *
 * Copyright (c) 2013, Luca Carlon
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the author nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LC_LOGGING_QT_H
#define LC_LOGGING_QT_H

#ifdef QT_CORE_LIB
#include <QtGlobal>
#include <QString>
#include <QObject>
#endif

#ifdef QT_QML_LIB
#include <QQmlContext>
#endif

#ifdef QT_CORE_LIB
#include <QtGlobal>
#include <QString>

namespace lightlogger {

/*------------------------------------------------------------------------------
|    log_handler
+-----------------------------------------------------------------------------*/
/**
 * @brief log_handler Function to handle Qt debugging output.
 * @param type
 * @param msg
 */
void log_handler(QtMsgType type, const QMessageLogContext&, const QString& s);

/*------------------------------------------------------------------------------
|    log_handler_with_category
+-----------------------------------------------------------------------------*/
/**
 * @brief log_handler Function to handle Qt debugging output including the category.
 * @param type
 * @param msg
 */
void log_handler_with_category(QtMsgType type, const QMessageLogContext& c, const QString& s);

#endif // QT_CORE_LIB

#ifdef QT_QML_LIB
/*------------------------------------------------------------------------------
|    LC_QMLLogger
+-----------------------------------------------------------------------------*/
class LC_QMLLogger : public QObject
{
    Q_OBJECT
public:
    static LC_QMLLogger& instance() {
        static LC_QMLLogger instance;
        return instance;
    }

    Q_INVOKABLE void debug(QString s) const;
    Q_INVOKABLE bool verbose(QString s) const;
    Q_INVOKABLE bool info(QString s) const;
    Q_INVOKABLE bool warn(QString s) const;
    Q_INVOKABLE bool error(QString s) const;
    Q_INVOKABLE bool critical(QString s) const;

    static void registerObject(QQmlContext* context) {
        context->setContextProperty(QStringLiteral("logger"), &(instance()));
    }

private:
    LC_QMLLogger() : QObject() {}
};
#endif // QT_QML_LIB

}

#endif // LC_LOGGING_QT_H
