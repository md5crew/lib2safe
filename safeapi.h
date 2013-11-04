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
    void errorRaised(ulong id, quint16 code, QString text);
    void checkEmailComplete(ulong id, bool result);

public slots:
    ulong checkEmail(QString email);
    void freeWorker(ulong worker_id);

private slots:
    void networkError(const QString& text);

private:
    ulong ticker;
    QString host;
    QHash<ulong, SafeWorker*> workers;
    bool reportError(ulong id, const QJsonDocument& response);
    ulong getId();
};

#endif // SAFEAPI_H
