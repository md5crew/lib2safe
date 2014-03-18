#include "safeapi.h"

ulong SafeApi::pullFile(QString file_id, QString path,
                        ulong size, ulong offset) {
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_PULL_FILE);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
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
    worker->addParam(PARAM_TOKEN, this->apiState.token);
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
            qWarning() << "[" << worker_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(worker_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonObject response = reply.object().value("response").toObject();
        QJsonObject file_info = response.value("file").toObject();
        SafeFile info(file_info);
        emit pushFileComplete(worker_id, info);
        /* ----- */
    });

    this->connect(worker, &SafeWorker::progress,
                  [=](ulong bytes, ulong total_bytes) {
        emit pushFileProgress(worker_id, bytes, total_bytes);
    });

    routeFileWorker(worker);
    return worker_id;
}

ulong SafeApi::copyFile(QString file_id, QString dst_dir_id,
                        QString file_name, bool overwrite, bool versioned)
{
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_COPY_FILE);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_FILE_ID, file_id);
    worker->addParam(PARAM_DIR_ID, dst_dir_id);
    if(!file_name.isEmpty()) worker->addParam(PARAM_FILE_NAME, file_name);
    if(overwrite) worker->addParam(PARAM_OVERWRITE, overwrite);
    if(versioned) worker->addParam(PARAM_VERSIONED, versioned);
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
        emit copyFileComplete(worker_id, response.value("id").toString().toULong());
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::moveFile(QString file_id, QString dst_dir_id,
                        QString file_name, bool overwrite, bool versioned)
{
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_MOVE_FILE);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_FILE_ID, file_id);
    worker->addParam(PARAM_DIR_ID, dst_dir_id);
    if(!file_name.isEmpty()) worker->addParam(PARAM_FILE_NAME, file_name);
    if(overwrite) worker->addParam(PARAM_OVERWRITE, overwrite);
    if(versioned) worker->addParam(PARAM_VERSIONED, versioned);
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
        emit moveFileComplete(worker_id, response.value("id").toString().toULong());
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::removeFile(QString file_id, bool now)
{
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_REMOVE_FILE);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_FILE_ID, file_id);
    if(now) worker->addParam(PARAM_REMOVE_NOW, TRUE);
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
        emit removeFileComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::makeDir(QString parent_dir_id, QString dir_name,
                       QJsonDocument props, ulong ctime, ulong mtime)
{
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_MAKE_DIR);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_DIR_ID, parent_dir_id);
    worker->addParam(PARAM_DIR_NAME, dir_name);
    if(!props.isEmpty()) worker->addParam(PARAM_PROPERTIES, props.toJson());
    if(ctime > 0) worker->addParam(PARAM_CREATE_TIME, (quint32)ctime);
    if(mtime > 0) worker->addParam(PARAM_MODIFY_TIME, (quint32)mtime);
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
        emit makeDirComplete(worker_id, response.value("dir_id").toString().toULong());
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::copyDir(QString src_dir_id, QString dst_dir_id,
                       QString dir_name, bool overwrite, bool versioned)
{
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_COPY_DIR);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_SRC_DIR_ID, src_dir_id);
    worker->addParam(PARAM_DST_DIR_ID, dst_dir_id);
    if(!dir_name.isEmpty()) worker->addParam(PARAM_DIR_NAME, dir_name);
    if(overwrite) worker->addParam(PARAM_OVERWRITE, overwrite);
    if(versioned) worker->addParam(PARAM_VERSIONED, versioned);
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
        emit copyDirComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::moveDir(QString src_dir_id, QString dst_dir_id,
                       QString dir_name, bool overwrite, bool versioned)
{
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_MOVE_DIR);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_SRC_DIR_ID, src_dir_id);
    worker->addParam(PARAM_DST_DIR_ID, dst_dir_id);
    if(!dir_name.isEmpty()) worker->addParam(PARAM_DIR_NAME, dir_name);
    if(overwrite) worker->addParam(PARAM_OVERWRITE, overwrite);
    if(versioned) worker->addParam(PARAM_VERSIONED, versioned);
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
        emit moveDirComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::removeDir(QString dir_id, bool recursive, bool now)
{
    ulong worker_id = getId();
    SafeWorker *worker = createFileWorker(CALL_REMOVE_DIR);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_DIR_ID, dir_id);
    if(recursive) worker->addParam(PARAM_RECURSIVE, TRUE);
    if(now) worker->addParam(PARAM_REMOVE_NOW, TRUE);
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
        emit removeDirComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::listDir(QString dir_id) {
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_LIST_DIR);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_DIR_ID, dir_id);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        //qDebug() << "DATA:" << data << "DATA LEN:" << data.length();
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
        QList<SafeDir> dirs;
        QList<SafeFile> files;
        QJsonObject response = reply.object().value("response").toObject();
        QJsonObject root = response.value("root").toObject();
        QJsonArray list_dirs = response.value("list_dirs").toArray();
        foreach(auto d, list_dirs) {
            SafeDir dd(d.toObject());
            dirs.append(dd);
        }
        QJsonArray list_files = response.value("list_files").toArray();
        foreach(auto f, list_files) {
            SafeFile ff(f.toObject());
            files.append(ff);
        }

        emit listDirComplete(worker_id, dirs, files, root);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::getProps(QString id_or_url, bool is_url)
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_GET_PROPS);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    if(is_url) {
        worker->addParam(PARAM_URL, id_or_url);
    } else {
        worker->addParam(PARAM_OBJECT_ID, id_or_url);
    }
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
        emit getPropsComplete(worker_id, response);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::setProps(QString object_id, QJsonDocument props)
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_SET_PROPS);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_OBJECT_ID, object_id);
    worker->addParam(PARAM_PROPERTIES, props.toJson());
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
        emit setPropsComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::getParentTree(QString dir_id)
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_GET_TREE);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_DIR_ID, dir_id);
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
        QJsonArray tree = reply.object().value("tree").toArray();
        emit getParentTreeComplete(worker_id, tree);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::getEvents(ulong after, bool last)
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_GET_EVENTS);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
    worker->addParam(PARAM_EVENTS_AFTER, (quint32)after);
    if(last) worker->addParam(PARAM_LAST_EVENT, TRUE);
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
        QJsonArray events = reply.object().value("response")
                .toObject().value("events").toArray();
        emit getEventsComplete(worker_id, events);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::purgeTrash()
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_PURGE_TRASH);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
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
        emit purgeTrashComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

