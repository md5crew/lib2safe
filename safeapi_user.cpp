#include "safeapi.h"

ulong SafeApi::checkEmail(QString email)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = new SafeWorker(this->host);
    workers.insert(worker_id, worker);

    this->connect(worker, &SafeWorker::done, [=](const QByteArray& data) {
        freeWorker(worker_id);

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "JSON ERROR: " << json_error.errorString();
            return;
        } else if(reportError(call_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonValue response = reply.object().value("response");
        emit checkEmailComplete(call_id, response.toObject().
                    value("available").toString() == TRUE);
        /* ----- */
    });

   this->connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker_id);
        networkError(text);
    });

    /* PARAM & CALL */
    worker->addParam(PARAM_EMAIL, email);
    worker->call(CALL_CHECK_EMAIL);
    return call_id;
}
