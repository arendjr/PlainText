#include "apicommand.h"

#include "conversionutil.h"
#include "realm.h"


#define super AdminCommand

ApiCommand::ApiCommand(QObject *parent) :
    super(parent) {
}

ApiCommand::~ApiCommand() {
}

void ApiCommand::prepareExecute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    m_requestId = takeWord();
}

void ApiCommand::sendReply(const QVariant &variant) {

    QString data = ConversionUtil::toJsonString(variant);
    if (data.isEmpty()) {
        data = "\"\"";
    }
    send(QString("{ "
                 "\"requestId\": \"%1\", "
                 "\"errorCode\": 0, "
                 "\"errorMessage\": \"\", "
                 "\"data\": %2 "
                 "}").arg(m_requestId, data));
}

void ApiCommand::sendError(int errorCode, const QString &errorMessage) {

    send(QString("{ "
                 "\"requestId\": \"%1\", "
                 "\"errorCode\": %2, "
                 "\"errorMessage\": %3, "
                 "\"data\": null "
                 "}").arg(m_requestId, QString::number(errorCode),
                          ConversionUtil::jsString(errorMessage)));
}
