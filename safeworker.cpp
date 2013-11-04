#include "safeworker.h"
#include "safecalls.h"
#include <QDebug>
#include <QtNetwork/QNetworkRequest>

SafeWorker::SafeWorker(QString host)
{
    this->host = host;
    manager = new QNetworkAccessManager();
}

SafeWorker::~SafeWorker()
{
    manager->deleteLater();
}

void SafeWorker::call(QString cmd)
{
    if(!manager) {
        return;
    } else {
        params.addQueryItem(PARAM_CMD, cmd);
    }

    QNetworkRequest req("https://" + this->host + "/");
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply = manager->post(req, params.query(QUrl::FullyEncoded).toUtf8());

    connect(reply, &QNetworkReply::sslErrors, [=](QList<QSslError> ssl_errors){
        qDebug() << "SSL ERRORS DETECTED: \n" << ssl_errors;
    });

    connect(reply, &QNetworkReply::readyRead, [=](){
        emit done(reply->readAll());
    });

    connect(reply, &QNetworkReply::finished, [=](){
        if(reply->error()) {
            emit error(reply->errorString());
        }
    });
}

void SafeWorker::addParam(QString name, QString value)
{
    params.addQueryItem(name, value);
}

