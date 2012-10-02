#include "searchcommand.h"

#include "exit.h"


#define super Command

SearchCommand::SearchCommand(QObject *parent) :
    super(parent) {

    setDescription("Search the area for secret exits. It may take multiple searches to find an "
                   "exit.\n"
                   "\n"
                   "Example: search");
}

SearchCommand::~SearchCommand() {
}

void SearchCommand::execute(Player *player, const QString &command) {

    super::execute(player, command);

    if (player->secondsStunned() > 0) {
        player->send(QString("Please wait %1 seconds.").arg(player->secondsStunned()), Olive);
        return;
    }

    CharacterStats stats = player->totalStats();
    bool isWanderer = (player->characterClass()->name() == "wanderer");

    int searchSkill = qMin(stats.intelligence + stats.faith, 100) + (isWanderer ? 30 : 0);

    Exit *foundExit = nullptr;
    for (const GameObjectPtr &exitPtr : currentArea()->exits()) {
        Exit *exit = exitPtr.cast<Exit *>();
        if (exit->isHidden()) {
            if (qrand() % 300 < searchSkill) {
                foundExit = exit;
                break;
            }
        }
    }

    if (foundExit) {
        send(QString("You found an exit: %1.").arg(foundExit->name()));
    } else {
        send("You didn't find anything.");
        player->stun(isWanderer ? 3000 : 4000);
    }
}
