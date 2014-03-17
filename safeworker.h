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
    void setCmd(QString cmd) { this->cmd = cmd; }
    QString getCmd() { return this->cmd; }
    void setFilepath(QString filepath) { this->filepath = filepath; }
    void setId(ulong call_id) { this->id = call_id; }
    ulong getId() { return this->id; }
    QHash<QString, QString> getCookies() const { return cookies; }

signals:
    void done(SafeWorker *worker, QByteArray data);
    void progress(ulong bytes, ulong total_bytes);
    void error(QString text);

public slots:
    void run();
    void pushFile();
    void pullFile();
    void addParam(QString name, QVariant value);

private:
    QString host;
    QString cmd;
    QString filepath;
    ulong id;
    QUrlQuery params;
    QNetworkAccessManager *manager;
    QHash<QString, QString> cookies; // C IS FOR COOKIE, THAT's GOOD ENOUGH FOR ME
};

#endif // SAFEWORKER_H
