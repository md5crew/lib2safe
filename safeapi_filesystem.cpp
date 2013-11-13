#include "safeapi.h"

ulong SafeApi::pullFile(QString file_id, QString path,
                        ulong size, ulong offset) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_PULL_FILE);
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->addParam(PARAM_FILE_ID, file_id);
    if(size > 0) worker->addParam(PARAM_SIZE, (quint32)size);
    if(offset > 0) worker->addParam(PARAM_OFFSET, (quint32)offset);
    worker->setId(worker_id);
    worker->setFilepath(path);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeFileWorker(worker_id);
        processFileWorkersQueue();
        emit pullFileComplete(worker_id);
    });

    this->connect(worker, &SafeWorker::progress,
                  [=](ulong bytes, ulong total_bytes) {
        emit pullFileProgress(worker_id, bytes, total_bytes);
    });

    routeFileWorker(worker);
    return worker_id;
}

ulong SafeApi::pushFile(QString dst_dir_id, QString path, QString file_name,
               bool overwrite, bool versioned, QJsonDocument props,
               ulong ctime, ulong mtime) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_PUSH_FILE);
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->addParam(PARAM_DIR_ID, dst_dir_id);
    worker->addParam(PARAM_FILE_NAME, file_name);
    if(overwrite) worker->addParam(PARAM_OVERWRITE, overwrite);
    if(versioned) worker->addParam(PARAM_VERSIONED, versioned);
    if(!props.isEmpty()) worker->addParam(PARAM_PROPERTIES, props.toJson());
    if(ctime > 0) worker->addParam(PARAM_CREATE_TIME, (quint32)ctime);
    if(mtime > 0) worker->addParam(PARAM_MODIFY_TIME, (quint32)mtime);
    worker->setId(worker_id);
    worker->setFilepath(path);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeFileWorker(worker_id);
        processFileWorkersQueue();

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
        QJsonObject file_info = response.value("file").toObject();
        emit pushFileComplete(worker_id, file_info);
        /* ----- */
    });

    this->connect(worker, &SafeWorker::progress,
                  [=](ulong bytes, ulong total_bytes) {
        emit pushFileProgress(worker_id, bytes, total_bytes);
    });

    routeFileWorker(worker);
    return worker_id;
}

ulong SafeApi::listDir(QString dir_id) {
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_LIST_DIR);
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->addParam(PARAM_DIR_ID, dir_id);
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
        QJsonObject root = response.value("root").toObject();
        QJsonArray list_dirs = response.value("list_dirs").toArray();
        QJsonArray list_files = response.value("list_files").toArray();

        emit listDirComplete(worker_id, list_dirs, list_files, root);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}
