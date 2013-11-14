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

void SafeWorker::run()
{
    if(!manager) {
        return;
    } else {
        params.addQueryItem(PARAM_CMD, this->cmd);
    }

    QNetworkRequest req("https://" + this->host + "/");
    req.setHeader(QNetworkRequest::UserAgentHeader, "lib2safe/0.1");
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply = manager->post(req, params.query(QUrl::FullyEncoded).toUtf8());

    connect(reply, &QNetworkReply::sslErrors, [=](QList<QSslError> ssl_errors){
        qDebug() << "SSL ERRORS DETECTED: \n" << ssl_errors;
    });

    connect(reply, &QNetworkReply::readyRead, [=](){
        emit done(this, reply->readAll());
    });

    connect(reply, &QNetworkReply::metaDataChanged, [=](){
        QVariant cookie_header = reply->header(QNetworkRequest::SetCookieHeader);
        if(cookie_header.isValid() &&
                cookie_header.canConvert<QList<QNetworkCookie>>()) {
            /* Store any cookie key-value pairs if given */
            foreach(auto cookie, cookie_header.value<QList<QNetworkCookie>>()) {
                auto kv = cookie.toRawForm(QNetworkCookie::NameAndValueOnly).split('=');
                cookies.insert(kv[0], kv[1]);
            }
        }
    });

    connect(reply, &QNetworkReply::finished, [=](){
        if(reply->error()) {
            emit error(reply->errorString());
        }
        reply->deleteLater();
    });
}

void SafeWorker::pushFile()
{
    if(!manager) {
        return;
    } else {
        params.addQueryItem(PARAM_CMD, CALL_PUSH_FILE);
    }

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QFile *file = new QFile(this->filepath);
    if(!file->open(QIODevice::ReadOnly)) {
        qWarning() << "[" << this->id << "] Unable to open" << file->fileName() << "for reading";
        delete file;
        delete multiPart;
        return;
    }
    file->setParent(multiPart);
    qDebug() << "Opened" << QFileInfo(this->filepath).absoluteFilePath();

    foreach(auto param, params.queryItems()) {
        QHttpPart metaPart;
        metaPart.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
        metaPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                           QVariant("form-data; name=\"" + param.first + "\""));
        metaPart.setBody(param.second.toUtf8());
        multiPart->append(metaPart);
    }

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader,
                       QVariant("application/octet-stream"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"file\"; filename=\"deadbeef\""));
    filePart.setBodyDevice(file);
    multiPart->append(filePart);

    QNetworkRequest req("https://" + this->host + "/");
    req.setHeader(QNetworkRequest::UserAgentHeader, "lib2safe/0.1");
    QNetworkReply *reply = manager->post(req, multiPart);
    multiPart->setParent(reply);

    connect(reply, &QNetworkReply::sslErrors, [=](QList<QSslError> ssl_errors) {
        qDebug() << "SSL ERRORS DETECTED: \n" << ssl_errors;
    });

    connect(reply, &QNetworkReply::readyRead, [=](){
        emit done(this, reply->readAll());
    });

    connect(reply, &QNetworkReply::uploadProgress, [=](ulong bytes, ulong total_bytes) {
        emit progress(bytes, total_bytes);
    });

    connect(reply, &QNetworkReply::finished, [=]() {
        file->close();
        if(reply->error()) {
            emit error(reply->errorString());
        }
        reply->deleteLater();
    });
}

void SafeWorker::pullFile()
{
    if(!manager) {
        return;
    } else {
        params.addQueryItem(PARAM_CMD, CALL_PULL_FILE);
    }

    QFile *file = new QFile(this->filepath);
    if(file->exists()) file->remove();
    if(!file->open(QIODevice::Append)) {
        qWarning() << "[" << this->id << "] Unable to open" << file->fileName() << "for writing";
        delete file;
        return;
    }

    qDebug() << "Opened" << QFileInfo(this->filepath).absoluteFilePath();

    QNetworkRequest req("https://" + this->host + "/");
    req.setHeader(QNetworkRequest::UserAgentHeader, "lib2safe/0.1");
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply = manager->post(req, params.query(QUrl::FullyEncoded).toUtf8());
    file->setParent(reply);

    connect(reply, &QNetworkReply::sslErrors, [=](QList<QSslError> ssl_errors) {
        qDebug() << "SSL ERRORS DETECTED: \n" << ssl_errors;
    });

    connect(reply, &QNetworkReply::readyRead, [=](){
        file->write(reply->readAll());
        file->flush();
    });

    connect(reply, &QNetworkReply::downloadProgress, [=](ulong bytes, ulong total_bytes) {
        emit progress(bytes, total_bytes);
    });

    connect(reply, &QNetworkReply::finished, [=]() {
        file->close();
        if(reply->error()) {
            emit error(reply->errorString());
        } else {
            emit done(this, 0);
        }
        reply->deleteLater();
    });
}

void SafeWorker::addParam(QString name, QVariant value)
{
    params.addQueryItem(name, value.toString());
}

