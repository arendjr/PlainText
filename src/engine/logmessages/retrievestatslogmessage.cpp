#include "retrievestatslogmessage.h"

#include <QDate>
#include <QFile>
#include <QMap>
#include <QStringList>

#include "asyncreplyevent.h"
#include "diskutil.h"
#include "realm.h"
#include "conversionutil.h"


RetrieveStatsLogMessage::RetrieveStatsLogMessage(Player *recipient, const QString &requestId,
                                                 const QString &type, int numDays) :
    LogMessage(),
    m_recipient(recipient),
    m_requestId(requestId),
    m_type(type),
    m_numDays(numDays) {
}

RetrieveStatsLogMessage::~RetrieveStatsLogMessage() {
}

void RetrieveStatsLogMessage::log() {

    QMap<QString, int> statData;

    QDate toDate = QDate::currentDate();
    QDate fromDate = toDate.addDays(-m_numDays);

    QString logDir = DiskUtil::logDir();
    while (fromDate <= toDate) {
        QString path = logDir + "/" + fromDate.toString("yyyyMMdd") + "/" + m_type + "stats";
        if (QFile::exists(path)) {
            QFile file(path);
            file.open(QIODevice::ReadOnly);
            while (!file.atEnd()) {
                QString line = file.readLine();
                QStringList parts = line.split(' ', QString::SkipEmptyParts);
                if (parts.length() == 3) {
                    QString key = parts[1];
                    int value = parts[2].toInt();
                    if (statData.contains(key)) {
                        statData[key] += value;
                    } else {
                        statData[key] = value;
                    }
                }
            }
        }

        fromDate = fromDate.addDays(1);
    }

    QStringList stringList;
    for (const QString &key : statData.keys()) {
        stringList << ConversionUtil::jsString(key) + ": " + QString::number(statData[key]);
    }

    QString reply = QString("{ "
                            "\"requestId\": \"%1\", "
                            "\"errorCode\": 0, "
                            "\"errorMessage\": \"\", "
                            "\"data\": { %2 } "
                            "}").arg(m_requestId, stringList.join(", "));

    Realm::instance()->enqueueEvent(new AsyncReplyEvent(m_recipient, reply));
}
