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
