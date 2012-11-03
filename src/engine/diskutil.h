#ifndef DISKUTIL_H
#define DISKUTIL_H

#include <QStringList>


class DiskUtil {

    public:
        static bool writeFile(const QString &path, const QString &content);

        static bool writeGameObject(const QString &objectType, uint id, const QString &content);

        static QString dataDir();

        static QStringList dataDirFileList(const QString &subdirectory = "/");

        static QString gameObjectPath(const QString &objectType, uint id);

        static void appendToLogFile(const QString &fileName, const QString &line);

        static QString logDir();
};

#endif // DISKUTIL_H
