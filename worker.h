#ifndef NETWORK_WORKER_H
#define NETWORK_WORKER_H

#include <QtNetwork/QNetworkAccessManager>
#include <QStringList>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkCookieJar>
#include <QtNetwork/QNetworkCookie>
#include <QtNetwork/QNetworkProxy>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJSEngine>
#include <QTimer>
#include "logger.h"
#define HTTP_OK 200
#define HTTP_FOUND 302

namespace app
{
class scheduler;
}

namespace network
{
extern const QString steam_login_base;

class worker : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit worker(const QString& username, const QString& password, QObject *parent = 0);
    enum STATE {NOT_LOGGED_IN = 0, STEAM_GUARD = 1, LOGIN_COMPLETE = 2, LOGGED_IN = 3, LOGIN_FAILED = 10};
    STATE get_state();
signals:
    void login_failure();
    void login_complete();
    void new_data(QNetworkReply* data);
public slots:
    void start_login();
private slots:
    void iterate_login(QNetworkReply* data);
    void job_finished(QNetworkReply *data);
    void job_timed_out();
private:
    STATE state;
    quint8 active_job_count;
    QMap <QTimer*, QNetworkReply*>active_requests;
    app::scheduler* controller;
    QStringList login_buffer;
};

} // namespace network

#endif // NETWORK_WORKER_H
