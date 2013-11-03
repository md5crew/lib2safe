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

signals:
    void errorRaised(long id, quint16 code, QString text);
    void checkEmailComplete(long id, bool result);

public slots:
    long checkEmail(QString email);
    void freeWorker(int worker_id);

private slots:
    void networkError(const QString& text);

private:
    long ticker;
    QString host;
    QList<SafeWorker*> workers;
    bool reportError(long id, const QJsonDocument& response);
    long getId();
};

#endif // SAFEAPI_H
