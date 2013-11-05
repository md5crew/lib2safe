#include "safeapi.h"

ulong SafeApi::getCaptcha()
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = createWorker(CALL_GET_CAPTCHA);
    worker->setId(worker_id);

    connect(worker, &SafeWorker::done,
            [=](const SafeWorker *w, const QByteArray& data) {
        /* LOGIC */
        SafeCaptcha captcha;
        if(w->getCookies().contains("captcha2safe")) {
            captcha.id = w->getCookies().value("captcha2safe");
        } else {
            qDebug() << "[" << call_id
                     << "] no PARAM_CAPTCHA_ID in cookies:\n" << w->getCookies();
        }

        freeWorker(worker_id);
        processWorkerQueue();
        captcha.picture = data;
        emit getCaptchaComplete(call_id, captcha);
        /* ----- */
    });

    routeWorker(worker);
    return call_id;
}

ulong SafeApi::checkEmail(QString email)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = createWorker(CALL_CHECK_EMAIL);
    worker->addParam(PARAM_EMAIL, email);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkerQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << call_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(call_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonValue response = reply.object().value("response");
        bool available = (response.toObject().value("available").toString() == TRUE);
        emit checkEmailComplete(call_id, available);
        /* ----- */
    });

    routeWorker(worker);
    return call_id;
}

ulong SafeApi::checkLogin(QString login)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = createWorker(CALL_CHECK_LOGIN);
    worker->addParam(PARAM_LOGIN, login);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkerQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << call_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(call_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonValue response = reply.object().value("response");
        bool available = (response.toObject().value("available").toString() == TRUE);
        emit checkLoginComplete(call_id, available);
        /* ----- */
    });

    routeWorker(worker);
    return call_id;
}

ulong SafeApi::registerUser(QString login, QString password,
                            QString user_captcha, SafeApi::SafeCaptcha captcha)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = createWorker(CALL_REGISTER);
    worker->addParam(PARAM_LOGIN, login);
    worker->addParam(PARAM_PASSWORD, password);
    worker->addParam(PARAM_CAPTCHA, user_captcha);
    worker->addParam(PARAM_CAPTCHA_ID, captcha.id);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkerQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << call_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(call_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonValue response = reply.object().value("response");
        this->lastLogin = login;
        this->lastRootDir = response.toObject().value("root_dir").toString();
        this->lastUserId = response.toObject().value("user_id").toString();
        emit registerUserComplete(call_id, this->lastRootDir, this->lastUserId);
        /* ----- */
    });

    routeWorker(worker);
    return call_id;
}

ulong SafeApi::unregisterUser(QString login, QString password)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = createWorker(CALL_UNREGISTER);
    worker->addParam(PARAM_LOGIN, login);
    worker->addParam(PARAM_PASSWORD, password);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkerQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << call_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(call_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonValue response = reply.object().value("response");
        this->clearState();
        QString user_login = response.toObject().value("user").toObject().value("login").toString();
        QString user_id = response.toObject().value("user").toObject().value("id").toString();
        emit unregisterUserComplete(call_id, user_login, user_id);
        /* ----- */
    });

    routeWorker(worker);
    return call_id;
}

ulong SafeApi::authUser(QString login, QString password)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = createWorker(CALL_AUTH);
    worker->addParam(PARAM_LOGIN, login);
    worker->addParam(PARAM_PASSWORD, password);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkerQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << call_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(call_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonValue response = reply.object().value("response");
        this->lastLogin = login;
        this->lastUserId = response.toObject().value("id").toString();
        this->lastToken = response.toObject().value("token").toString();
        emit authUserComplete(call_id, this->lastUserId);
        /* ----- */
    });

    routeWorker(worker);
    return call_id;
}

ulong SafeApi::authUserCaptcha(QString login, QString password,
                               QString user_captcha, SafeApi::SafeCaptcha captcha)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = createWorker(CALL_AUTH);
    worker->addParam(PARAM_LOGIN, login);
    worker->addParam(PARAM_PASSWORD, password);
    worker->addParam(PARAM_CAPTCHA, user_captcha);
    worker->addParam(PARAM_CAPTCHA_ID, captcha.id);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkerQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << call_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(call_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonValue response = reply.object().value("response");
        this->lastLogin = login;
        this->lastUserId = response.toObject().value("id").toString();
        this->lastToken = response.toObject().value("token").toString();
        emit authUserComplete(call_id, this->lastUserId);
        /* ----- */
    });

    routeWorker(worker);
    return call_id;
}

ulong SafeApi::logoutUser()
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = createWorker(CALL_LOGOUT);
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkerQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << call_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(call_id, reply)) {
            return;
        }

        /* LOGIC */
        this->clearState();
        emit logoutUserComplete(call_id);
        /* ----- */
    });

    routeWorker(worker);
    return call_id;
}

ulong SafeApi::getDiskQuota()
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = createWorker(CALL_GET_DISK_QUOTA);
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkerQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << call_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(call_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonValue response = reply.object().value("response");

        ulong usedBytes = response.toObject().value("quotas")
                .toObject().value("used_bytes").toDouble();
        ulong totalBytes = response.toObject().value("quotas")
                .toObject().value("total_bytes").toDouble();
        emit getDiskQuotaComplete(call_id, usedBytes, totalBytes);
        /* ----- */
    });

    routeWorker(worker);
    return call_id;
}

ulong SafeApi::getPersonal()
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = createWorker(CALL_GET_PERSONAL);
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkerQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << call_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(call_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonValue response = reply.object().value("response");
        QJsonObject personal = response.toObject().value("personal").toObject();
        QJsonObject props = response.toObject().value("props").toObject();
        emit getPersonalComplete(call_id, personal, props);
        /* ----- */
    });

    routeWorker(worker);
    return call_id;
}

ulong SafeApi::setPersonal(QJsonDocument personal, QJsonDocument props)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = createWorker(CALL_SET_PERSONAL);
    worker->addParam(PARAM_PERSONAL, personal.toJson());
    worker->addParam(PARAM_PROPERTIES, props.toJson());
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkerQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << call_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(call_id, reply)) {
            return;
        }

        /* LOGIC */
        emit setPersonalComplete(call_id);
        /* ----- */
    });

    routeWorker(worker);
    return call_id;
}

ulong SafeApi::setPersonalEmail(QJsonDocument personal, QJsonDocument props, QString password)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = createWorker(CALL_SET_PERSONAL);
    worker->addParam(PARAM_PERSONAL, personal.toJson());
    worker->addParam(PARAM_PROPERTIES, props.toJson());
    worker->addParam(PARAM_PASSWORD, password);
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkerQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << call_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(call_id, reply)) {
            return;
        }

        /* LOGIC */
        emit setPersonalComplete(call_id);
        /* ----- */
    });

    routeWorker(worker);
    return call_id;
}

ulong SafeApi::changePassword(QString login, QString password, QString new_password)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = createWorker(CALL_CHANGE_PASSWORD);
    worker->addParam(PARAM_LOGIN, login);
    worker->addParam(PARAM_PASSWORD, password);
    worker->addParam(PARAM_NEW_PASSWORD, new_password);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkerQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << call_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(call_id, reply)) {
            return;
        }

        /* LOGIC */
        emit changePasswordComplete(call_id);
        /* ----- */
    });

    routeWorker(worker);
    return call_id;
}

ulong SafeApi::activatePromo(QString code)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = createWorker(CALL_ACTIVATE_PROMO);
    worker->addParam(PARAM_PROMO_CODE, code);
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->setId(worker_id);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);
        processWorkerQueue();

        QJsonParseError json_error;
        QJsonDocument reply = QJsonDocument::fromJson(data, &json_error);
        if(json_error.error) {
            qDebug() << "[" << call_id
                     << "] JSON error:" << json_error.errorString();
            return;
        } else if(reportError(call_id, reply)) {
            return;
        }

        /* LOGIC */
        QJsonValue response = reply.object().value("response");
        ulong used_bytes = response.toObject().value("quotas")
                .toObject().value("used_bytes").toDouble();
        ulong total_bytes = response.toObject().value("quotas")
                .toObject().value("total_bytes").toDouble();
        ulong added = response.toObject().value("quotas")
                .toObject().value("difference").toDouble();
        emit activatePromoComplete(call_id, used_bytes, total_bytes, added);
        /* ----- */
    });

    routeWorker(worker);
    return call_id;
}




