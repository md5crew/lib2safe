#ifndef SAFEAPI_H
#define SAFEAPI_H

#include <QObject>
#include <QtMath>
#include "safeworker.h"
#include "safecalls.h"
#include "safeerrors.h"

class SafeApi : public QObject
{
    Q_OBJECT
public:
    SafeApi(QString host);

    struct SafeCaptcha {
        QByteArray picture;
        QString id;
    };

signals:
    void errorRaised(ulong id, quint16 code, QString text);

    /* USER */
    void getCaptchaComplete(ulong id, SafeCaptcha captcha);
    void checkEmailComplete(ulong id, bool available);
    void checkLoginComplete(ulong id, bool available);
    void registerUserComplete(ulong id, QString root_dir, QString user_id);
    void unregisterUserComplete(ulong id, QString login, QString user_id);
    void authUserComplete(ulong id, QString user_id);
    void logoutUserComplete(ulong id);
    void getDiskQuotaComplete(ulong id, ulong used_bytes, ulong total_bytes);
    void getPersonalComplete(ulong id, QJsonObject personal, QJsonObject props);
    void setPersonalComplete(ulong id);
    void changePasswordComplete(ulong id);
    void activatePromoComplete(ulong id, ulong used_bytes, ulong total_bytes, ulong added);

public slots:
    void freeWorker(ulong worker_id);
    void clearState();

    /* USER */
    ulong getCaptcha();
    ulong checkEmail(QString email);
    ulong checkLogin(QString login);
    ulong registerUser(QString login, QString password, QString user_captcha, SafeCaptcha captcha);
    ulong unregisterUser(QString login, QString password);
    ulong authUser(QString login, QString password);
    ulong authUserCaptcha(QString login, QString password, QString user_captcha, SafeCaptcha captcha);
    ulong logoutUser();
    ulong getDiskQuota();
    ulong getPersonal();
    ulong setPersonal(QJsonDocument personal, QJsonDocument props);
    ulong setPersonalEmail(QJsonDocument personal, QJsonDocument props, QString password);
    ulong changePassword(QString login, QString password, QString new_password);
    ulong activatePromo(QString code);

private slots:
    void networkError(const QString& text);

private:
    ulong ticker;
    QString host;
    QHash<ulong, SafeWorker*> workers;

    /* memory */
    QString lastToken;
    QString lastUserId;
    QString lastRootDir;
    QString lastLogin;

    /* methods */
    bool reportError(ulong id, const QJsonDocument& response);
    ulong getId();
};

#endif // SAFEAPI_H
