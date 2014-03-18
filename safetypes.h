#include <QObject>
#include <QJsonObject>
#include <QDebug>

#ifndef SAFETYPES_H
#define SAFETYPES_H

struct SafeCaptcha {
    QByteArray picture;
    QString id;
};

struct SafeApiState {
    QString token;
    QString userId;
    ulong tokenTimestamp;

    void clear() {
        token.clear();
        userId.clear();
        tokenTimestamp = 0;
    }
};

struct SafeDir {
    QString id;
    QString name;
    QString special_dir;
    QString creator;
    QString owner;
    QString c_type;
    QJsonObject props;
    ulong ctime;
    ulong mtime;
    ulong uptime;
    bool is_trash;
    bool shared;
    bool versioned;

    SafeDir(QJsonObject obj) {
        fromJsonObject(obj);
    }

    void fromJsonObject(QJsonObject obj) {
        id = obj.value("id").toString();
        name = obj.value("name").toString();
        special_dir = obj.value("special_dir").toString();
        creator = obj.value("creator").toString();
        owner = obj.value("owner").toString();
        c_type = obj.value("c_type").toString();
        props = obj.value("props").toObject();
        ctime = obj.value("ctime").toDouble();
        mtime = obj.value("mtime").toDouble();
        uptime = obj.value("uptime").toDouble();
        is_trash = (obj.value("is_trash").toDouble() == 1) ? true : false;
        shared = (obj.value("shared").toDouble() == 1) ? true : false;
        versioned = (obj.value("versioned").toDouble() == 1) ? true : false;
    }

    QString toString() const {
        return this->name;
    }
};

struct SafeFile {
    QString id;
    QString name;
    QString creator;
    QString owner;
    QString c_type;
    QJsonObject props;
    QString current_version;
    QString chksum;
    QString sha256;
    QString thumbnail;
    ulong size;
    ulong uptime;
    ulong ctime;
    ulong mtime;
    ulong total_size;
    ulong version_count;
    bool is_trash;
    bool shared;
    bool versioned;

    SafeFile(QJsonObject obj) {
        fromJsonObject(obj);
    }

    void fromJsonObject(QJsonObject obj) {
        id = obj.value("id").toString();
        name = obj.value("name").toString();
        creator = obj.value("creator").toString();
        owner = obj.value("owner").toString();
        c_type = obj.value("c_type").toString();
        props = obj.value("props").toObject();
        current_version = obj.value("current_version").toString();
        chksum = obj.value("chksum").toString();
        sha256 = obj.value("sha256").toString();
        thumbnail = obj.value("thumbnail").toString();
        size = obj.value("size").toDouble();
        uptime = obj.value("uptime").toDouble();
        ctime = obj.value("ctime").toDouble();
        mtime = obj.value("mtime").toDouble();
        total_size = obj.value("total_size").toDouble();
        version_count = obj.value("version_count").toDouble();
        is_trash = (obj.value("is_trash").toDouble() == 1) ? true : false;
        shared = (obj.value("shared").toDouble() == 1) ? true : false;
        versioned = (obj.value("versioned").toDouble() == 1) ? true : false;
    }

    QString toString() const {
        return this->name;
    }
};

#endif // SAFETYPES_H
