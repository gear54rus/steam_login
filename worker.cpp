#include "worker.h"
#include "util.h"
#define MAX_ACTIVE_JOBS 5

namespace network
{
const QString steam_login_base("https://store.steampowered.com/login/");

worker::worker(const QString& username, const QString& password, QObject *parent) :
    QNetworkAccessManager(parent)
{
   //QNetworkProxy::setApplicationProxy(QNetworkProxy(QNetworkProxy::HttpProxy,"localhost",8888));
    this->state = NOT_LOGGED_IN;
    this->active_job_count = 0;
    this->login_buffer.clear();
    this->login_buffer.push_back(username);
    this->login_buffer.push_back(password);
    QTimer* current;
    for (quint8 i = 0; i < MAX_ACTIVE_JOBS; i++)
    {
        current = new QTimer(this);
        current->setSingleShot(true);
        QObject::connect(current, SIGNAL(timeout()), this, SLOT(job_timed_out()));
        this->active_requests.insert(current, 0);
    }
}
worker::STATE worker::get_state()
{
    return this->state;
}
void worker::start_login()
{
#ifdef TRADE_BOT_NOLOGIN
    this->state = network::worker::LOGGED_IN;
    emit login_complete();
#else
    QObject::disconnect(this, SIGNAL(finished(QNetworkReply*)));
    QObject::connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(iterate_login(QNetworkReply*)));
    app::log(QString("Starting login process for: %1...").arg(this->login_buffer[0]));
    QNetworkCookie steam_timezone;
    QNetworkRequest request;
    steam_timezone.setDomain(".steampowered.com");
    steam_timezone.setPath("/");
    steam_timezone.setName("timezoneOffset");
    steam_timezone.setValue((QString::number(util::getTimeZoneOffset().toInt() * 3600) + ",0").toUtf8());
    this->cookieJar()->insertCookie(steam_timezone);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=utf-8");
    request.setUrl(steam_login_base + "getrsakey");
    this->post(request, QString("username=%1&l=english").arg(this->login_buffer[0]).toUtf8());
#endif
}
void worker::iterate_login(QNetworkReply* data)
{
    QByteArray response = data->readAll();
    QJsonDocument response_json = QJsonDocument::fromJson(response);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=utf-8");
    try
    {
        switch (this->state)
        {
        case NOT_LOGGED_IN:
        {
            if (!response_json.object().value("success").toBool())
            {
                throw QString("Success: false");
            }
            this->state = STATE(int(this->state) + 1);
            this->login_buffer.push_back(util::get_encrypted_password(this->login_buffer[1], response_json.object().value("publickey_mod").toString(), response_json.object().value("publickey_exp").toString()));
            this->login_buffer.push_back(response_json.object().value("timestamp").toString());
            request.setUrl(steam_login_base + "dologin");
            this->post(request, QString("username=%1&password=%2&rsatimestamp=%3").arg(this->login_buffer[0], util::percent_encode(this->login_buffer[2]), this->login_buffer[3]).toUtf8());
            break;
        }
        case STEAM_GUARD:
        {
            if (response_json.object().value("message").toString() != "SteamGuard")
            {
                throw response_json.object().value("message").toString();
            }
            this->state = STATE(int(this->state) + 1);
            this->login_buffer.push_back(app::query(QString("Enter Steam Guard code for %1: ").arg(this->login_buffer[0])).toUpper()); //4
            this->login_buffer.push_back(response_json.object().value("emailsteamid").toString()); //5
            request.setUrl(steam_login_base + "dologin");
            this->post(request, QString("username=%1&password=%2&emailauth=%3&emailsteamid=%4&rsatimestamp=%5").arg(this->login_buffer[0], util::percent_encode(this->login_buffer[2]), this->login_buffer[4], this->login_buffer[5], this->login_buffer[3]).toUtf8());
            break;
        }
        case LOGIN_COMPLETE:
        {
            if (!response_json.object().value("login_complete").toBool())
            {
                throw response_json.object().value("message").toString();
            }
            this->state = STATE(int(this->state) + 1);
            this->login_buffer.push_back(response_json.object().value("transfer_parameters").toObject().value("token").toString()); //6
            this->login_buffer.push_back(response_json.object().value("transfer_parameters").toObject().value("webcookie").toString()); //7
            request.setUrl(steam_login_base + "transfer");
            this->post(request, QString("steamid=%1&token=%2&remember_login=%3&webcookie=%4").arg(this->login_buffer[5], this->login_buffer[6], "true", this->login_buffer[7]).toUtf8());
            break;
        }
        case LOGGED_IN:
        {
            QNetworkCookie temp_cookie;
            QList<QNetworkCookie> cookies = this->cookieJar()->cookiesForUrl(QUrl(steam_login_base));
            for (int i = 0; i < cookies.size(); i++)
            {
                temp_cookie = cookies[i];
                temp_cookie.setDomain(".steamcommunity.com");
                this->cookieJar()->insertCookie(temp_cookie);
            }
            QObject::disconnect(this, SLOT(iterate_login(QNetworkReply*)));
            for (quint8 i = 0; i < 6; i++)
            {
                this->login_buffer.removeLast();
            }
            app::log(QString("Successfully logged in as: %1").arg(this->login_buffer[0]));
            emit login_complete();
            break;
        }
        case LOGIN_FAILED:
        {
            Q_UNREACHABLE();
        }
        }
    }
    catch (QString reason)
    {
        app::log(QString("%1: login failure at stage %2: %3").arg(this->login_buffer[0], QString::number((int)this->state), reason), app::LOG_ERR);
        QObject::disconnect(this, SLOT(iterate_login(QNetworkReply*)));
        this->state = LOGIN_FAILED;
        emit login_failure();
    }
    data->deleteLater();
}
void worker::job_finished(QNetworkReply *data)
{
    QTimer* timer = this->active_requests.key(data);
    timer->stop();
    if (data->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == HTTP_FOUND)
    {
        QNetworkRequest new_request(data->request());
        new_request.setUrl(data->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl());
        this->active_requests[timer] = this->get(new_request);
        timer->start();
        data->deleteLater();
    }
    else
    {
        this->active_requests[timer] = 0;
        this->active_job_count--;
        emit new_data(data);
        //this->check_jobs();
    }
}
void worker::job_timed_out()
{
#ifdef WORKER_TIMEOUT_DUMP
    {
        QFile f("timeout_dump.txt");
        f.open(QFile::Append);
        QTextStream s(&f);
        s << QDateTime::currentDateTimeUtc().toString();
    }
#endif
    this->active_requests[(QTimer*)QObject::sender()]->abort();
}
} // namespace network
