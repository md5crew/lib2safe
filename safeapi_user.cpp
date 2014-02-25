#include "safeapi.h"

ulong SafeApi::getCaptcha()
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_GET_CAPTCHA);
    worker->setId(worker_id);

    connect(worker, &SafeWorker::done,
            [=](const SafeWorker *w, const QByteArray& data) {
        /* LOGIC */
        SafeCaptcha captcha;
        if(w->getCookies().contains("captcha2safe")) {
            captcha.id = w->getCookies().value("captcha2safe");
        } else {
            qDebug() << "[" << worker_id
                     << "] no PARAM_CAPTCHA_ID in cookies:\n" << w->getCookies();
        }

        freeWorker(worker_id);
        processWorkersQueue();
        captcha.picture = data;
        emit getCaptchaComplete(worker_id, captcha);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::checkEmail(QString email)
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_CHECK_EMAIL);
    worker->addParam(PARAM_EMAIL, email);
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
        QJsonValue response = reply.object().value("response");
        bool available = (response.toObject().value("available").toString() == TRUE);
        emit checkEmailComplete(worker_id, available);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::checkLogin(QString login)
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_CHECK_LOGIN);
    worker->addParam(PARAM_LOGIN, login);
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
        QJsonValue response = reply.object().value("response");
        bool available = (response.toObject().value("available").toString() == TRUE);
        emit checkLoginComplete(worker_id, available);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::registerUser(QString login, QString password,
                            QString user_captcha, SafeCaptcha captcha)
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_REGISTER);
    worker->addParam(PARAM_LOGIN, login);
    worker->addParam(PARAM_PASSWORD, password);
    worker->addParam(PARAM_CAPTCHA, user_captcha);
    worker->addParam(PARAM_CAPTCHA_ID, captcha.id);
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
        QJsonValue response = reply.object().value("response");
        emit registerUserComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::unregisterUser(QString login, QString password)
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_UNREGISTER);
    worker->addParam(PARAM_LOGIN, login);
    worker->addParam(PARAM_PASSWORD, password);
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
        QJsonValue response = reply.object().value("response");
        this->clearState();
        QString user_login = response.toObject().value("user").toObject().value("login").toString();
        QString user_id = response.toObject().value("user").toObject().value("id").toString();
        emit unregisterUserComplete(worker_id, user_login, user_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::authUser(QString login, QString password)
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_AUTH);
    worker->addParam(PARAM_LOGIN, login);
    worker->addParam(PARAM_PASSWORD, password);
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
        QJsonValue response = reply.object().value("response");
        this->apiState.userId = response.toObject().value("id").toString();
        this->apiState.token = response.toObject().value("token").toString();
        this->apiState.tokenTimestamp = QDateTime::currentDateTime().toTime_t();
        emit authUserComplete(worker_id, this->apiState.userId);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::authUserCaptcha(QString login, QString password,
                               QString user_captcha, SafeCaptcha captcha)
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_AUTH);
    worker->addParam(PARAM_LOGIN, login);
    worker->addParam(PARAM_PASSWORD, password);
    worker->addParam(PARAM_CAPTCHA, user_captcha);
    worker->addParam(PARAM_CAPTCHA_ID, captcha.id);
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
        QJsonValue response = reply.object().value("response");
        this->apiState.userId = response.toObject().value("id").toString();
        this->apiState.token = response.toObject().value("token").toString();
        this->apiState.tokenTimestamp = QDateTime::currentDateTime().toTime_t();
        emit authUserComplete(worker_id, this->apiState.userId);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::logoutUser()
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_LOGOUT);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
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
        this->clearState();
        emit logoutUserComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::getDiskQuota()
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_GET_DISK_QUOTA);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
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
        QJsonValue response = reply.object().value("response");

        ulong usedBytes = response.toObject().value("quotas")
                .toObject().value("used_bytes").toDouble();
        ulong totalBytes = response.toObject().value("quotas")
                .toObject().value("total_bytes").toDouble();
        emit getDiskQuotaComplete(worker_id, usedBytes, totalBytes);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::getPersonal()
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_GET_PERSONAL);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
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
        QJsonValue response = reply.object().value("response");
        QJsonObject personal = response.toObject().value("personal").toObject();
        QJsonObject props = response.toObject().value("props").toObject();
        emit getPersonalComplete(worker_id, personal, props);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::setPersonal(QJsonDocument personal, QJsonDocument props)
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_SET_PERSONAL);
    worker->addParam(PARAM_PERSONAL, personal.toJson());
    worker->addParam(PARAM_PROPERTIES, props.toJson());
    worker->addParam(PARAM_TOKEN, this->apiState.token);
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
        emit setPersonalComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::setPersonalEmail(QJsonDocument personal, QJsonDocument props, QString password)
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_SET_PERSONAL);
    worker->addParam(PARAM_PERSONAL, personal.toJson());
    worker->addParam(PARAM_PROPERTIES, props.toJson());
    worker->addParam(PARAM_PASSWORD, password);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
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
        emit setPersonalComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::changePassword(QString login, QString password, QString new_password)
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_CHANGE_PASSWORD);
    worker->addParam(PARAM_LOGIN, login);
    worker->addParam(PARAM_PASSWORD, password);
    worker->addParam(PARAM_NEW_PASSWORD, new_password);
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
        emit changePasswordComplete(worker_id);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}

ulong SafeApi::activatePromo(QString code)
{
    ulong worker_id = getId();
    SafeWorker *worker = createWorker(CALL_ACTIVATE_PROMO);
    worker->addParam(PARAM_PROMO_CODE, code);
    worker->addParam(PARAM_TOKEN, this->apiState.token);
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
        QJsonValue response = reply.object().value("response");
        ulong used_bytes = response.toObject().value("quotas")
                .toObject().value("used_bytes").toDouble();
        ulong total_bytes = response.toObject().value("quotas")
                .toObject().value("total_bytes").toDouble();
        ulong added = response.toObject().value("quotas")
                .toObject().value("difference").toDouble();
        emit activatePromoComplete(worker_id, used_bytes, total_bytes, added);
        /* ----- */
    });

    routeWorker(worker);
    return worker_id;
}




