#include "safeapi.h"

ulong SafeApi::lockObject(QString id, ulong timeout,
                 bool exclusive, QJsonObject userdata) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_LOCK);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_OBJECT_ID, id);
    worker->addParam(PARAM_LOCK_TIMEOUT, (quint32)timeout);
    if(exclusive) worker->addParam(PARAM_LOCK_EXCLUSIVE, exclusive ? "1" : "0");
    if(!userdata.isEmpty()) worker->addParam(PARAM_LOCK_USERDATA, userdata);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkersQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qWarning() << "[" << worker_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(worker_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonObject response = reply.object().value("response").toObject();
        emit lockObjectComplete(worker_id, response.value("lock_token").toString());
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::unlockObject(QString lock_token) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_UNLOCK);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_LOCK_TOKEN, lock_token);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkersQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qWarning() << "[" << worker_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(worker_id, reply)) {
            return;
        }

        /* LOGIC */
        emit unlockObjectComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::listLocks(QString id) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_LIST_LOCKS);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_OBJECT_ID, id);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkersQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qWarning() << "[" << worker_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(worker_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonObject response = reply.object().value("response").toObject();
        emit listLocksComplete(worker_id, response.value("list_locks").toArray());
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::refreshLock(QString lock_token, ulong timeout) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_REFRESH_LOCK);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_LOCK_TOKEN, lock_token);
    worker->addParam(PARAM_LOCK_TIMEOUT, (quint32)timeout);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkersQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qWarning() << "[" << worker_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(worker_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonObject response = reply.object().value("response").toObject();
        QJsonObject userdata = response.value("userdata").toObject();
        bool exclusive = (response.value("exclusive").toString() == "0") ? false : true;

        emit refreshLockComplete(worker_id, userdata, exclusive);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}
