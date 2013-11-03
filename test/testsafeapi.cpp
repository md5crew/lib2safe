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
        connect(api, &SafeApi::checkEmailComplete, [=](long id, bool available){
            qDebug() << "["<< id << "] Checked aviability:" << available;
            delete api;
            if(--n < 1) { emit stop(); }
        });

        connect(api, &SafeApi::errorRaised, [=](long id, quint16 code, QString text){
            qDebug() << "[" << id << "] Error checking email:" << text;
            delete api;
            if(--n < 1) { emit stop(); }
        });

        api->checkEmail("me@kc.vc");
    }

    void checkEmailTaken()
    {
        auto api = new SafeApi(API_HOST);
        connect(api, &SafeApi::checkEmailComplete, [=](long id, bool available){
            qDebug() << "["<< id << "] Checked aviability:" << available;
            delete api;
            if(--n < 1) { emit stop(); }
        });

        connect(api, &SafeApi::errorRaised, [=](long id, quint16 code, QString text){
            qDebug() << "[" << id << "] Error checking email:" << text;
            delete api;
            if(--n < 1) { emit stop(); }
        });

        api->checkEmail("not.me.lol@kc.vc");
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    auto test = new TestSafeApi();
    a.connect(test, &TestSafeApi::stop, &QCoreApplication::quit);
    test->setN(2);
    test->checkEmailFree(); // me@kc.vc
    test->checkEmailTaken(); // not.me.lol@kc.vc
    return a.exec();
}

#include "testsafeapi.moc"
