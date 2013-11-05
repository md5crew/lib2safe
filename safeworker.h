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

    void setCmd(QString cmd) { this->cmd = cmd; }
    void setId(ulong call_id) { this->id = call_id; }
    ulong getId() { return this->id; }
    QHash<QString, QString> getCookies() const { return cookies; }

signals:
    void done(SafeWorker *worker, QByteArray data);
    void error(QString text);

public slots:
    void run();
    void addParam(QString name, QString value);

private:
    QString host;
    QString cmd;
    ulong id;
    QUrlQuery params;
    QNetworkAccessManager *manager;
    QHash<QString, QString> cookies; // C IS FOR COOKIE, THAT's GOOD ENOUGH FOR ME
};

#endif // SAFEWORKER_H
