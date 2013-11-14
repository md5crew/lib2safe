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

    void checkEmail()
    {
        int k;
        auto api = new SafeApi(API_HOST);
        connect(api, &SafeApi::checkEmailComplete, [=, &k](ulong id, bool available){
            qDebug() << "["<< id << "] Checked aviability:" << available;
            if(--k < (0-1)) {
                delete api;
                if(--n < 1) { emit stop(); }
            }
        });

        connect(api, &SafeApi::errorRaised, [=](ulong id, quint16 code, QString text){
            qDebug() << "[" << id << "] Error:" << text;
            delete api;
            if(--n < 1) { emit stop(); }
        });

        api->checkEmail("me@kc.vc");
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

    void listDirs()
    {
        int k;
        auto api = new SafeApi(API_HOST);
        connect(api, &SafeApi::authUserComplete, [=](ulong id, const QString& user_id){
            qDebug() << "["<< id << "] Auth user complete (id):" << user_id;
            api->listDir(); // /
            api->listDir("372492033760"); // /Музыка/
        });

        connect(api, &SafeApi::listDirComplete, [=, &k](
                ulong id, QJsonArray dirs,
                QJsonArray files,
                QJsonObject root_info){
            qDebug() << "["<< id << "] Listing directory ("
                     << root_info.value("tree").toString() << "):"
                     << "\n+++++PARENT+++++";
            qDebug() << root_info;

            qDebug() << "\n+++++DIRS+++++";
            foreach(QJsonValue dir, dirs) {
                qDebug() << dir.toObject();
            }

            qDebug() << "\n+++++FILES+++++";
            foreach(QJsonValue file, files) {
                qDebug() << file.toObject();
            }

            if(--k < (0-1)) {
                delete api;
                if(--n < 1) { emit stop(); }
            }
        });

        connect(api, &SafeApi::errorRaised, [=](ulong id, quint16 code, QString text){
            qDebug() << "[" << id << "] Error:" << text;
            delete api;
            if(--n < 1) { emit stop(); }
        });

        api->authUser("md5@kc.vc", "12345678");
    }

    void getFile()
    {
        auto api = new SafeApi(API_HOST);
        connect(api, &SafeApi::authUserComplete, [=](ulong id, const QString& user_id){
            qDebug() << "["<< id << "] Auth user complete (id):" << user_id;
            api->pullFile("372493033760", "08 Водичка.mp3"); // '/Музыка/08 Водичка.mp3'
        });

        connect(api, &SafeApi::pullFileComplete, [=](ulong id){
            qDebug() << "["<< id << "] Sucessfully pulled file"
                     << "'08 Водичка.mp3'";
            delete api;
            if(--n < 1) { emit stop(); }
        });

        connect(api, &SafeApi::pullFileProgress, [=](ulong id, ulong bytes, ulong total_bytes){
            float percentage = ((float)bytes) / total_bytes * 100;
            qDebug() << "["<< id << "] File download progress:" << percentage
                     << "(" << bytes << "/" << total_bytes << ")";
        });

        connect(api, &SafeApi::errorRaised, [=](ulong id, quint16 code, QString text){
            qDebug() << "[" << id << "] Error:" << text;
            delete api;
            if(--n < 1) { emit stop(); }
        });

        api->authUser("md5@kc.vc", "12345678");
    }

    void putFile()
    {
        auto api = new SafeApi(API_HOST);
        connect(api, &SafeApi::authUserComplete, [=](ulong id, const QString& user_id){
            qDebug() << "["<< id << "] Auth user complete (id):" << user_id;
            api->pushFile("227930033757",
                          "/Users/xlab/Documents/jackpot.mp4",
                          "jackpot.mp4"); // '/jackpot.mp4'
        });

        connect(api, &SafeApi::pushFileComplete, [=](ulong id, const QJsonObject& file_info){
            qDebug() << "["<< id << "] Sucessfully pushed file"
                     << file_info;
            delete api;
            if(--n < 1) { emit stop(); }
        });

        connect(api, &SafeApi::pushFileProgress, [=](ulong id, ulong bytes, ulong total_bytes){
            float percentage = ((float)bytes) / total_bytes * 100;
            qDebug() << "["<< id << "] File upload progress:" << percentage
                     << "(" << bytes << "/" << total_bytes << ")";
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
    test->setN(1);
    //test->checkEmail();
    //test->getCaptcha();
    //test->getDiskQuota();
    //test->listDirs();
    test->getFile();
    //test->putFile();
    return a.exec();
}

#include "testsafeapi.moc"
