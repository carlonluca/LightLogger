#include <QDebug>

namespace lightlogger {

#ifdef QT_CORE_LIB
/*------------------------------------------------------------------------------
|    log_handler
+-----------------------------------------------------------------------------*/
/**
 * @brief log_handler Function to handle Qt debugging output.
 * @param type
 * @param msg
 */
void log_handler(QtMsgType type, const QMessageLogContext&, const QString& s)
{
    switch (type) {
    case QtDebugMsg:
        log_verbose(qPrintable(s));
        break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    case QtInfoMsg:
        log_info(qPrintable(s));
        break;
#endif
    case QtWarningMsg:
        log_warn(qPrintable(s));
        break;
    case QtCriticalMsg:
        log_err(qPrintable(s));
        break;
    case QtFatalMsg:
        log_critical(qPrintable(s));
        break;
    }
}

/*------------------------------------------------------------------------------
|    log_handler_with_category
+-----------------------------------------------------------------------------*/
/**
 * @brief log_handler Function to handle Qt debugging output including the category.
 * @param type
 * @param msg
 */
void log_handler_with_category(QtMsgType type, const QMessageLogContext& c, const QString& s)
{
    switch (type) {
    case QtDebugMsg:
        log_verbose_t(c.category, qPrintable(s));
        break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    case QtInfoMsg:
        log_info_t(c.category, qPrintable(s));
        break;
#endif
    case QtWarningMsg:
        log_warn_t(c.category, qPrintable(s));
        break;
    case QtCriticalMsg:
        log_err_t(c.category, qPrintable(s));
        break;
    case QtFatalMsg:
        log_critical_t(c.category, qPrintable(s));
        break;
    }
}

#endif // QT_CORE_LIB

#ifdef QT_QML_LIB
/*------------------------------------------------------------------------------
|    LC_QMLLogger
+-----------------------------------------------------------------------------*/
void LC_QMLLogger::debug(QString s) const
{ DEF_FUNC(debug)(qPrintable(s)); }

bool LC_QMLLogger::verbose(QString s) const
{ return DEF_FUNC(verbose);(qPrintable(s)); }

bool LC_QMLLogger::info(QString s) const
{ return DEF_FUNC(info)(qPrintable(s)); }

bool LC_QMLLogger::warn(QString s) const
{ return DEF_FUNC(warn)(qPrintable(s)); }

bool LC_QMLLogger::error(QString s) const
{ return DEF_FUNC(err)(qPrintable(s)); }

bool LC_QMLLogger::critical(QString s) const
{ return DEF_FUNC(err)(qPrintable(s)); }

#endif // QT_QML_LIB

}
