#include "safeapi.h"

ulong SafeApi::listVersions(QString id) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_LIST_VERSIONS);
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->addParam(PARAM_OBJECT_ID, id);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkersQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << worker_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(worker_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonObject response = reply.object().value("response").toObject();
        QJsonArray versions = response.value("versions").toArray();
        QString current = response.value("current").toString();
        QString file_id = response.value("file_id").toString();
        emit listVersionsComplete(worker_id, versions, current, file_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::getVersion(QString id) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_GET_VERSION);
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->addParam(PARAM_OBJECT_ID, id);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkersQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << worker_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(worker_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonObject response = reply.object().value("response").toObject();
        emit getVersionComplete(worker_id, response.value("current_version").toString());
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::setVersion(QString id, QString version_id) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_SET_VERSION);
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->addParam(PARAM_OBJECT_ID, id);
    worker->addParam(PARAM_VERSION_ID, version_id);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkersQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << worker_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(worker_id, reply)) {
            return;
        }

        /* LOGIC */
        emit setVersionComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::removeVersion(QString id) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_REMOVE_VERSION);
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->addParam(PARAM_OBJECT_ID, id);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkersQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << worker_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(worker_id, reply)) {
            return;
        }

        /* LOGIC */
        emit removeVersionComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::setVersionControl(QString id, bool enabled) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(
                enabled ? CALL_SET_VERSIONS_ON : CALL_SET_VERSIONS_OFF);
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->addParam(PARAM_OBJECT_ID, id);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkersQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << worker_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(worker_id, reply)) {
            return;
        }

        /* LOGIC */
        emit setVersionControlComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}
