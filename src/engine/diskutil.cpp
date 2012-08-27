#include "diskutil.h"

#include <unistd.h>

#include <QDebug>
#include <QDir>
#include <QFile>


bool DiskUtil::writeFile(const QString &path, const QString &content) {

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file" << file.fileName() << "for writing.";
        return false;
    }

    file.write(content.toUtf8());
    file.flush();
#ifdef Q_OS_LINUX
    fdatasync(file.handle());
#else
    fsync(file.handle());
#endif
    return true;
}

bool DiskUtil::writeGameObject(const char *objectType, uint id, const QString &content) {

    return writeFile(gameObjectPath(objectType, id), content);
}

QString DiskUtil::dataDir() {

    return qgetenv("PT_DATA_DIR");
}

QStringList DiskUtil::dataDirFileList(const QString &subdirectory) {

    QDir dir(DiskUtil::dataDir() + subdirectory);
    return dir.entryList(QDir::Files);
}

QString DiskUtil::gameObjectPath(const char *objectType, uint id) {

    return dataDir() + QString("/%1.%2").arg(objectType).arg(id, 9, 10, QChar('0'));
}
