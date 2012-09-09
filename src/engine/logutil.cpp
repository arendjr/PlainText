#include "logutil.h"

#include "commandlogmessage.h"
#include "diskutil.h"
#include "npctalklogmessage.h"
#include "realm.h"
#include "sessionlogmessage.h"


void LogUtil::logCommand(const QString &playerName, const QString &command) {

    if (DiskUtil::logDir().isEmpty()) {
        return;
    }

    Realm::instance()->enqueueLogMessage(new CommandLogMessage(playerName, command));
}

void LogUtil::logNpcTalk(const QString &npcName, const QString &message) {

    if (DiskUtil::logDir().isEmpty()) {
        return;
    }

    Realm::instance()->enqueueLogMessage(new NpcTalkLogMessage(npcName, message));
}

void LogUtil::logSessionEvent(const QString &source, const QString &message) {

    if (DiskUtil::logDir().isEmpty()) {
        return;
    }

    Realm::instance()->enqueueLogMessage(new SessionLogMessage(source, message));
}
