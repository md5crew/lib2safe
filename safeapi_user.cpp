#include "safeapi.h"

ulong SafeApi::getCaptcha()
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = new SafeWorker(this->host);
    workers.insert(worker_id, worker);

    this->connect(worker, &SafeWorker::done,
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
        captcha.picture = data;
        emit getCaptchaComplete(call_id, captcha);
        /* ----- */
    });

    this->connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker_id);
        networkError(text);
    });

    /* PARAMS & CALL */
    worker->call(CALL_GET_CAPTCHA);
    return call_id;
}

ulong SafeApi::checkEmail(QString email)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = new SafeWorker(this->host);
    workers.insert(worker_id, worker);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);

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

    this->connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker_id);
        networkError(text);
    });

    /* PARAMS & CALL */
    worker->addParam(PARAM_EMAIL, email);
    worker->call(CALL_CHECK_EMAIL);
    return call_id;
}

ulong SafeApi::checkLogin(QString login)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = new SafeWorker(this->host);
    workers.insert(worker_id, worker);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);

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

    this->connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker_id);
        networkError(text);
    });

    /* PARAMS & CALL */
    worker->addParam(PARAM_LOGIN, login);
    worker->call(CALL_CHECK_LOGIN);
    return call_id;
}

ulong SafeApi::registerUser(QString login, QString password,
                            QString user_captcha, SafeApi::SafeCaptcha captcha)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = new SafeWorker(this->host);
    workers.insert(worker_id, worker);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);

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

    this->connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker_id);
        networkError(text);
    });

    /* PARAMS & CALL */
    worker->addParam(PARAM_LOGIN, login);
    worker->addParam(PARAM_PASSWORD, password);
    worker->addParam(PARAM_CAPTCHA, user_captcha);
    worker->addParam(PARAM_CAPTCHA_ID, captcha.id);
    worker->call(CALL_REGISTER);
    return call_id;
}

ulong SafeApi::unregisterUser(QString login, QString password)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = new SafeWorker(this->host);
    workers.insert(worker_id, worker);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);

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

    this->connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker_id);
        networkError(text);
    });

    /* PARAMS & CALL */
    worker->addParam(PARAM_LOGIN, login);
    worker->addParam(PARAM_PASSWORD, password);
    worker->call(CALL_UNREGISTER);
    return call_id;
}

ulong SafeApi::authUser(QString login, QString password)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = new SafeWorker(this->host);
    workers.insert(worker_id, worker);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);

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

    this->connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker_id);
        networkError(text);
    });

    /* PARAMS & CALL */
    worker->addParam(PARAM_LOGIN, login);
    worker->addParam(PARAM_PASSWORD, password);
    worker->call(CALL_AUTH);
    return call_id;
}

ulong SafeApi::authUserCaptcha(QString login, QString password,
                               QString user_captcha, SafeApi::SafeCaptcha captcha)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = new SafeWorker(this->host);
    workers.insert(worker_id, worker);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);

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

    this->connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker_id);
        networkError(text);
    });

    /* PARAMS & CALL */
    worker->addParam(PARAM_LOGIN, login);
    worker->addParam(PARAM_PASSWORD, password);
    worker->addParam(PARAM_CAPTCHA, user_captcha);
    worker->addParam(PARAM_CAPTCHA_ID, captcha.id);
    worker->call(CALL_AUTH);
    return call_id;
}

ulong SafeApi::logoutUser()
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = new SafeWorker(this->host);
    workers.insert(worker_id, worker);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);

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

    this->connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker_id);
        networkError(text);
    });

    /* PARAMS & CALL */
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->call(CALL_LOGOUT);
    return call_id;
}

ulong SafeApi::getDiskQuota()
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = new SafeWorker(this->host);
    workers.insert(worker_id, worker);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);

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

    this->connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker_id);
        networkError(text);
    });

    /* PARAMS & CALL */
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->call(CALL_GET_DISK_QUOTA);
    return call_id;
}

ulong SafeApi::getPersonal()
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = new SafeWorker(this->host);
    workers.insert(worker_id, worker);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);

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

    this->connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker_id);
        networkError(text);
    });

    /* PARAMS & CALL */
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->call(CALL_GET_PERSONAL);
    return call_id;
}

ulong SafeApi::setPersonal(QJsonDocument personal, QJsonDocument props)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = new SafeWorker(this->host);
    workers.insert(worker_id, worker);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);

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

    this->connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker_id);
        networkError(text);
    });

    /* PARAMS & CALL */
    worker->addParam(PARAM_PERSONAL, personal.toJson());
    worker->addParam(PARAM_PROPERTIES, props.toJson());
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->call(CALL_SET_PERSONAL);
    return call_id;
}

ulong SafeApi::setPersonalEmail(QJsonDocument personal, QJsonDocument props, QString password)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = new SafeWorker(this->host);
    workers.insert(worker_id, worker);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);

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

    this->connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker_id);
        networkError(text);
    });

    /* PARAMS & CALL */
    worker->addParam(PARAM_PERSONAL, personal.toJson());
    worker->addParam(PARAM_PROPERTIES, props.toJson());
    worker->addParam(PARAM_PASSWORD, password);
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->call(CALL_SET_PERSONAL);
    return call_id;
}

ulong SafeApi::changePassword(QString login, QString password, QString new_password)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = new SafeWorker(this->host);
    workers.insert(worker_id, worker);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);

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

    this->connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker_id);
        networkError(text);
    });

    /* PARAMS & CALL */
    worker->addParam(PARAM_LOGIN, login);
    worker->addParam(PARAM_PASSWORD, password);
    worker->addParam(PARAM_NEW_PASSWORD, new_password);
    worker->call(CALL_CHANGE_PASSWORD);
    return call_id;
}

ulong SafeApi::activatePromo(QString code)
{
    ulong call_id = getId();
    ulong worker_id = ticker;
    SafeWorker *worker = new SafeWorker(this->host);
    workers.insert(worker_id, worker);

    this->connect(worker, &SafeWorker::done,
                  [=](const SafeWorker *w, const QByteArray& data) {
        freeWorker(worker_id);

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

    this->connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker_id);
        networkError(text);
    });

    /* PARAMS & CALL */
    worker->addParam(PARAM_PROMO_CODE, code);
    worker->addParam(PARAM_TOKEN, this->lastToken);
    worker->call(CALL_ACTIVATE_PROMO);
    return call_id;
}




