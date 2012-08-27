#ifndef DISKUTIL_H
#define DISKUTIL_H

#include <QStringList>


class DiskUtil {

    public:
        static bool writeFile(const QString &path, const QString &content);

        static bool writeGameObject(const char *objectType, uint id, const QString &content);

        static QString dataDir();

        static QStringList dataDirFileList(const QString &subdirectory = "/");

        static QString gameObjectPath(const char *objectType, uint id);
};

#endif // DISKUTIL_H
