#include "logger.h"

namespace app
{
LOG_LEVEL loglevel = LOG_MSG;
namespace
{
QMutex stdioLock;
QString levels[4] = {"MISC", "MSG", "ERROR", "FATAL"};
}
void log(const QString& message, LOG_LEVEL level)
{
    if (level >= app::loglevel)
    {
        QMutexLocker _lock(&stdioLock);
        ((level >= 2) ? (std::cerr) : (std::cout)) << (QString("%1 [%2] %3\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"), levels[level], message)).toStdString();
    }
}
QString query(const QString& query)
{
    QMutexLocker _lock(&stdioLock);
    std::string reply;
    std::cout << (QString("%1 [%2] %3").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"), "QUERY", query)).toStdString();
    std::cin >> reply;
    return QString::fromStdString(reply);
}
QStringList query(const QStringList& queries)
{
    QMutexLocker _lock(&stdioLock);
    std::string reply;
    QStringList results;
    for (int i = 0; i < queries.size(); i++)
    {
        std::cout << (QString("%1 [%2] %3").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"), "QUERY", queries[i])).toStdString();
        reply.clear();
        std::cin >> reply;
        results.push_back(QString::fromStdString(reply));
    }
    return results;
}
} // namespace app
