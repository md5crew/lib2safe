#include "safeapi.h"

SafeApi::SafeApi(QString host)
{
    this->ticker = 0;
    this->host = host;
}

void SafeApi::freeWorker(ulong worker_id)
{
    delete workers.value(worker_id);
    workers.remove(worker_id);
}

void SafeApi::clearState()
{
    this->lastToken.clear();
    this->lastUserId.clear();
    this->lastRootDir.clear();
    this->lastLogin.clear();
}

bool SafeApi::reportError(ulong id, const QJsonDocument& response)
{
    QJsonValue success = response.object().value("success");
    if(success.toString() == FALSE) {
        qDebug() << success << success.toBool() << false;
        int code = response.object().value("error_code").toString().toInt();
        QString text = response.object().value("error_msg").toString();
        emit errorRaised(id, code, text);
        return true;
    } else {
        // no error actually
        return false;
    }
}

ulong SafeApi::getId()
{
    ++ticker;
    return (QDateTime::currentMSecsSinceEpoch() << 32) + ticker;
}

void SafeApi::networkError(const QString& text)
{
    qDebug() << "[ERROR] network error:\n" << text;
}
