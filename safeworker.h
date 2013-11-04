#ifndef SAFEWORKER_H
#define SAFEWORKER_H

#include <QObject>
#include <QJsonDocument>
#include <QtNetwork>
#include <QUrlQuery>

class SafeWorker : public QObject
{
    Q_OBJECT
public:
    SafeWorker(QString host);
    ~SafeWorker();

signals:
    void done(SafeWorker *worker, QByteArray data);
    void error(QString text);

public slots:
    void call(QString cmd);
    void addParam(QString name, QString value);
    QHash<QString, QString> getCookies() const { return cookies; }

private:
    QString host;
    QUrlQuery params;
    QNetworkAccessManager *manager;
    QHash<QString, QString> cookies; // C IS FOR COOKIE, THAT's GOOD ENOUGH FOR ME
};

#endif // SAFEWORKER_H
