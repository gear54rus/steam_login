#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QDateTime>
#include <QJSEngine>
#include <QFile>
#include <QPair>

namespace util
{
extern QString web_api_key;
extern quint8 network_timeout;
void init_resources();
QString getTimeZoneOffset();
QString get_encrypted_password(const QString& password, const QString& publickey_mod, const QString& publickey_exp);
QString percent_encode(const QString &target);
template <class T>
bool in_range(const T& value, QPair<T, T>range)
{
    return (value >= range.first) && (value <= range.second);
}
}
#endif // UTIL_H
