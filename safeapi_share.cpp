#include "safeapi.h"

ulong SafeApi::shareObject(QString id, QString login,
                           bool writable, ulong expires) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_SHARE);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_OBJECT_ID, id);
    worker->addParam(PARAM_LOGIN, login);
    if(writable) worker->addParam(PARAM_SHARE_WRITABLE, TRUE);
    if(expires > 0) worker->addParam(PARAM_SHARE_EXPIRES, (quint32)expires);
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
        emit shareObjectComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::unshareObject(QString id, QString login) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_UNSHARE);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_OBJECT_ID, id);
    worker->addParam(PARAM_LOGIN, login);
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
        emit unshareObjectComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::unshareAll(QString id) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_UNSHARE_ALL);
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
        emit unshareAllComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::listShares(QString id) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_LIST_SHARES);
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
        emit listSharesComplete(worker_id, response.value("shares").toArray());
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::publicObject(QString id, ulong expires) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_PUBLIC);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_OBJECT_ID, id);
    if(expires > 0) worker->addParam(PARAM_SHARE_EXPIRES, (quint32)expires);
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
        emit publicObjectComplete(worker_id, response.value("link").toString());
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::unpublicObject(QString id) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_UNPUBLIC);
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
        emit unpublicObjectComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}
