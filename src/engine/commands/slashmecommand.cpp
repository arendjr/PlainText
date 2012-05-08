#include "slashmecommand.h"

#include "engine/util.h"


SlashMeCommand::SlashMeCommand(Character *character, QObject *parent) :
    Command(character, parent) {
}

SlashMeCommand::~SlashMeCommand() {
}

void SlashMeCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Try being a little bit more creative...")) {
        return;
    }

    QString action = takeRest();

    foreach (const GameObjectPtr &other, currentArea()->characters()) {
        other.cast<Character *>()->send(Util::colorize("%1 %2", Purple).arg(character()->name(), action));
    }
}
