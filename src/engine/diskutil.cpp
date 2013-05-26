#include "diskutil.h"

#include <unistd.h>

#include <QDate>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QTime>

#include "logutil.h"


bool DiskUtil::writeFile(const QString &path, const QString &content) {

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        LogUtil::logError("Could not open file %1 for writing", file.fileName());
        return false;
    }

    qint64 bytesWritten = file.write(content.toUtf8());
    file.flush();
    return (bytesWritten != -1);
}

bool DiskUtil::writeGameObject(const QString &objectType, uint id, const QString &content) {

    return writeFile(gameObjectPath(objectType, id), content);
}

QString DiskUtil::dataDir() {

    static QString path;
    static bool defined = false;

    if (!defined) {
        path = qgetenv("PT_DATA_DIR");
        defined = true;
    }
    return path;
}

QStringList DiskUtil::dataDirFileList(const QString &subdirectory) {

    QDir dir(DiskUtil::dataDir() + subdirectory);
    QStringList entries = dir.entryList(QDir::Files);
    return entries.filter(QRegExp("^[^.].*$")); // extra filter to remove files starting
                                                // with a dot on Windows
}

QString DiskUtil::gameObjectPath(const QString &objectType, uint id) {

    return dataDir() + QString("/%1.%2").arg(objectType.toLower()).arg(id, 9, 10, QChar('0'));
}

void DiskUtil::appendToLogFile(const QString &fileName, const QString &line) {

    static QDate today = QDate::currentDate();
    static QMap<QString, QFile *> openFiles;

    if (today != QDate::currentDate()) {
        for (QFile *file : openFiles) {
            file->close();
            delete file;
        }
        openFiles.clear();
        today = QDate::currentDate();
    }

    QFile *file;
    if (openFiles.contains(fileName)) {
        file = openFiles[fileName];
    } else {
        QString dirPath = logDir() + "/" + today.toString("yyyyMMdd");
        if (!QDir(dirPath).exists() && !QDir(logDir()).mkpath(today.toString("yyyyMMdd"))) {
            LogUtil::setLoggingEnabled(false);
            LogUtil::logError("Could not create log directory: %1\n"
                              "Logging system disabled.", dirPath);
            return;
        }

        QString path = dirPath + "/" + fileName;
        file = new QFile(path);
        if (!file->open(QIODevice::WriteOnly | QIODevice::Append)) {
            LogUtil::setLoggingEnabled(false);
            LogUtil::logError("Could not open log file: %1\n"
                              "Logging system disabled.", path);
            return;
        }

        openFiles[fileName] = file;
    }

    file->write(QString(QTime::currentTime().toString("HH:mm:ss.zzz ") + line + "\n").toUtf8());
    file->flush();
}

QString DiskUtil::logDir() {

    static QString path;
    static bool defined = false;

    if (!defined) {
        path = qgetenv("PT_LOG_DIR");
        defined = true;
    }
    return path;
}
