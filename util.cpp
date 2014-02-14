#include "util.h"
#include <iostream>

namespace
{
QString steam_login_script, steam_api_key;
}

namespace util
{
QString web_api_key;
quint8 network_timeout;
void init_resources()
{
    //load steam js
    QFile script("://scripts/steam_cipher.js");
    script.open(QFile::ReadOnly);
    steam_login_script = QString::fromLatin1(script.readAll());
}
QString getTimeZoneOffset()
{
    QDateTime dt1 = QDateTime::currentDateTime();
    QDateTime dt2 = dt1.toUTC();
    dt1.setTimeSpec(Qt::UTC);
    int offset = dt2.secsTo(dt1) / 3600;
    if (offset > 0)
        return QString("+%1").arg(offset);
    return QString("%1").arg(offset);
}
QString get_encrypted_password(const QString& password, const QString& publickey_mod, const QString& publickey_exp)
{
    QJSEngine login_engine;
    login_engine.globalObject().setProperty("navigator", login_engine.newObject());
    login_engine.globalObject().property("navigator").setProperty("appName", "Opera");
    login_engine.globalObject().setProperty("PKEY_MOD", publickey_mod);
    login_engine.globalObject().setProperty("PKEY_EXP", publickey_exp);
    login_engine.globalObject().setProperty("PWD", password);
    login_engine.evaluate(steam_login_script);
    return login_engine.globalObject().property("encryptedPassword").toString();
}
QString percent_encode(const QString &target)
{
    QString result(target);
    result.replace("=", "%3D");
    result.replace("/", "%2F");
    result.replace("+", "%2B");
    return result;
}
}
