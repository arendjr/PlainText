#include "logretrievecommand.h"

#include "diskutil.h"
#include "player.h"
#include "retrievestatslogmessage.h"
#include "realm.h"
#include "util.h"


#define super ApiCommand

LogRetrieveCommand::LogRetrieveCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-log-retrieve <request-id> stats <stats-type> <number-of-days>");
}

LogRetrieveCommand::~LogRetrieveCommand() {
}

void LogRetrieveCommand::execute(Character *character, const QString &command) {

    super::prepareExecute(character, command);

    if (!character->isPlayer()) {
        sendError(403, "Only players may retrieve logs");
        return;
    }

    if (takeWord() != "stats") {
        sendError(400, "First argument should be stats");
        return;
    }

    QString statsType = takeWord();
    int numDays = takeWord().toInt();

    if (DiskUtil::logDir().isEmpty()) {
        sendError(503, "Logging unavailable");
        return;
    }

    Player *player = qobject_cast<Player *>(character);
    Realm::instance()->enqueueLogMessage(new RetrieveStatsLogMessage(player, requestId(),
                                                                     statsType, numDays));
}
