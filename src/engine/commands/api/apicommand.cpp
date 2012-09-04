#include "apicommand.h"

#include "engine/conversionutil.h"
#include "engine/realm.h"


ApiCommand::ApiCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {
}

ApiCommand::~ApiCommand() {
}

void ApiCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();

    m_requestId = takeWord();
}

void ApiCommand::sendReply(const QVariant &variant) {

    send(QString("{ "
                 "\"requestId\": \"%1\", "
                 "\"errorCode\": 0, "
                 "\"errorMessage\": \"\", "
                 "\"data\": %2 "
                 "}").arg(m_requestId, ConversionUtil::toJSON(variant, DontIncludeTypeInfo)));
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


