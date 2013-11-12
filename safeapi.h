#ifndef SAFEAPI_H
#define SAFEAPI_H

#include <QObject>
#include <QtMath>
#include <QMutex>
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

    /* FILESYSTEM */
    void pullFileProgress(ulong id, ulong bytes, ulong total_bytes);
    void pullFileComplete(ulong id);
    void pushFileProgress(ulong id, ulong bytes, ulong total_bytes);
    void pushFileComplete(ulong id, QJsonObject file_info);
    void copyFileComplete(ulong id, ulong file_id);
    void moveFileComplete(ulong id, ulong file_id);
    void removeFileComplete(ulong id);
    void makeDirComplete(ulong id, ulong dir_id);
    void copyDirComplete(ulong id);
    void moveDirComplete(ulong id);
    void listDirComplete(ulong id, QJsonArray dirs,
                         QJsonArray files,
                         QJsonObject root_info);
    void removeDirComplete(ulong id);
    void getPropsComplete(ulong id, QJsonObject props);
    void setPropsComplete(ulong id);
    void getParentTreeComplete(ulong id, QJsonArray tree);
    void getEventsComplete(ulong id, QJsonArray events);
    void purgeTrashComplete(ulong id);


public slots:
    void freeWorker(ulong worker_id);
    void freeFileWorker(ulong worker_id);
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

    /* FILESYSTEM */
    ulong pullFile(QString file_id, QString path, ulong size = 0, ulong offset = 0);
    //    ulong pushFile(QString dst_dir_id, QString path, QString file_name,
    //                   bool overwrite = false, bool versioned = false,
    //                   QJsonDocument props = QJsonDocument(),
    //                   ulong ctime = 0, ulong mtime = 0);
    //    ulong copyFile(QString file_id, QString dst_dir_id, QString file_name,
    //                   bool overwrite = false, bool versioned = false);
    //    ulong moveFile(QString file_id, QString dst_dir_id, QString file_name,
    //                   bool overwrite = false, bool versioned = false);
    //    ulong removeFile(QString file_id, bool now = false);
    //    ulong makeDir(QString parent_dir_id, QString dir_name,
    //                  QJsonDocument props = QJsonDocument(),
    //                  ulong ctime = 0, ulong mtime = 0);
    //    ulong copyDir(QString src_dir_id, QString dst_dir_id, QString dir_name,
    //                  bool overwrite = false, bool versioned = false);
    //    ulong moveDir(QString src_dir_id, QString dst_dir_id, QString dir_name,
    //                  bool overwrite = false, bool versioned = false);
    //    ulong removeDir(QString dir_id, bool recursive = false, bool now = false);
    ulong listDir(QString dir_id = QString());
    //    ulong getProps(QString object_id);
    //    ulong setProps(QString object_id, QJsonDocument props);
    //    ulong getParentTree(QString dir_id);
    //    ulong getEvents(ulong after, bool last = false);
    //    ulong purgeTrash();

private slots:
    void networkError(const QString& text);

private:
    ulong ticker;
    QString host;
    int maxThreads;
    int maxFileThreads;
    QMutex mutex;

    QHash<ulong, SafeWorker*> workersPool;
    QQueue<SafeWorker*> workersQueue;
    QHash<ulong, SafeWorker*> fileWorkersPool;
    QQueue<SafeWorker*> fileWorkersQueue;

    /* memory */
    QString lastToken;
    QString lastUserId;
    QString lastRootDir;
    QString lastLogin;

    /* methods */
    SafeWorker *createWorker(QString cmd);
    SafeWorker *createFileWorker(QString cmd);
    void routeWorker(SafeWorker *worker);
    void routeFileWorker(SafeWorker *worker);
    void processWorkersQueue();
    void processFileWorkersQueue();
    bool reportError(ulong id, const QJsonDocument& response);
    ulong getId();
};

#endif // SAFEAPI_H
