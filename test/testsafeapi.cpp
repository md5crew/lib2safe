#include <QObject>
#include <QDebug>
#include <QtTest/QtTest>
#include <QCoreApplication>
#include "safeapi.h"

class TestSafeApi : public QObject
{
    Q_OBJECT

private:
    int n;
signals:
    void stop();
public slots:
    void setN(int n) { this->n = n; }

    void checkEmailFree()
    {
        auto api = new SafeApi(API_HOST);
        connect(api, &SafeApi::checkEmailComplete, [=](ulong id, bool available){
            qDebug() << "["<< id << "] Checked aviability:" << available;
            delete api;
            if(--n < 1) { emit stop(); }
        });

        connect(api, &SafeApi::errorRaised, [=](ulong id, quint16 code, QString text){
            qDebug() << "[" << id << "] Error:" << text;
            delete api;
            if(--n < 1) { emit stop(); }
        });

        api->checkEmail("me@kc.vc");
    }

    void checkEmailTaken()
    {
        auto api = new SafeApi(API_HOST);
        connect(api, &SafeApi::checkEmailComplete, [=](ulong id, bool available){
            qDebug() << "["<< id << "] Checked aviability:" << available;
            delete api;
            if(--n < 1) { emit stop(); }
        });

        connect(api, &SafeApi::errorRaised, [=](ulong id, quint16 code, QString text){
            qDebug() << "[" << id << "] Error:" << text;
            delete api;
            if(--n < 1) { emit stop(); }
        });

        api->checkEmail("not.me.lol@kc.vc");
    }

    void getCaptcha()
    {
        auto api = new SafeApi(API_HOST);
        connect(api, &SafeApi::getCaptchaComplete, [=](ulong id, SafeApi::SafeCaptcha captcha){
            qDebug() << "["<< id << "] Got captcha (id):" << captcha.id;
            delete api;
            if(--n < 1) { emit stop(); }
        });

        connect(api, &SafeApi::errorRaised, [=](ulong id, quint16 code, QString text){
            qDebug() << "[" << id << "] Error:" << text;
            delete api;
            if(--n < 1) { emit stop(); }
        });

        api->getCaptcha();
    }

    void getDiskQuota()
    {
        auto api = new SafeApi(API_HOST);
        connect(api, &SafeApi::authUserComplete, [=](ulong id, const QString& user_id){
            qDebug() << "["<< id << "] Auth user complete (id):" << user_id;
            api->getDiskQuota();
        });

        connect(api, &SafeApi::getDiskQuotaComplete,
                [=](ulong id, ulong used, ulong total){
            qDebug() << "["<< id
                     << "] Disk quota: (used:"<< used
                     <<"), (total:" << total << ")";
            delete api;
            if(--n < 1) { emit stop(); }
        });

        connect(api, &SafeApi::errorRaised, [=](ulong id, quint16 code, QString text){
            qDebug() << "[" << id << "] Error:" << text;
            delete api;
            if(--n < 1) { emit stop(); }
        });

        api->authUser("md5@kc.vc", "12345678");
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    auto test = new TestSafeApi();
    a.connect(test, &TestSafeApi::stop, &QCoreApplication::quit);
    test->setN(4);
    test->checkEmailFree(); // me@kc.vc
    test->checkEmailTaken(); // not.me.lol@kc.vc
    test->getCaptcha();
    test->getDiskQuota();
    return a.exec();
}

#include "testsafeapi.moc"
