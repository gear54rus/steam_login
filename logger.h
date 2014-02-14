#ifndef APP_LOGGER_H
#define APP_LOGGER_H

#include <QString>
#include <QStringList>
#include <QMutex>
#include <QMutexLocker>
#include <QDateTime>
#include <iostream>

namespace app
{
enum LOG_LEVEL {LOG_MISC = 0, LOG_MSG = 1, LOG_ERR = 2, LOG_CRIT = 3};
extern LOG_LEVEL loglevel;
void log(const QString& message, LOG_LEVEL level = LOG_MSG);
QString query(const QString& query);
QStringList query(const QStringList& queries);
} // namespace app

#endif // APP_LOGGER_H
