#include "safeapi.h"

SafeApi::SafeApi(QString host)
{
    this->ticker = 0;
    this->host = host;
}

void SafeApi::freeWorker(int worker_id)
{
    delete workers.at(worker_id);
}

bool SafeApi::reportError(long id, const QJsonDocument& response)
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

long SafeApi::getId()
{
    ++ticker;
    return (QDateTime::currentMSecsSinceEpoch() << 32) + ticker;
}

void SafeApi::networkError(const QString& text)
{
    qDebug() << "NETWORK ERROR: \n" << text;
}
