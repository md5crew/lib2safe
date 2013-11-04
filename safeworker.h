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
    void done(QByteArray data);
    void error(QString text);

public slots:
    void call(QString cmd);
    void addParam(QString name, QString value);

private:
    QString host;
    QUrlQuery params;
    QNetworkAccessManager *manager;
};

#endif // SAFEWORKER_H
