#include "safeapi.h"

long SafeApi::checkEmail(QString email)
{
    int worker_id = workers.count();
    long call_id = getId();
    SafeWorker *worker = new SafeWorker(this->host);
    workers.append(worker);

    this->connect(worker, &SafeWorker::done, [=](const QJsonDocument& response) {
        freeWorker(worker_id);
        if(reportError(call_id, response)) {
            return;
        }

        /* LOGIC */
        QJsonValue data = response.object().value("response");
        emit checkEmailComplete(call_id, data.toObject().
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
