#include "statscommand.h"

#include "engine/realm.h"
#include "engine/util.h"


StatsCommand::StatsCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Display your character stats.\n"
                   "\n"
                   "Example: stats");
}

StatsCommand::~StatsCommand() {
}

void StatsCommand::execute(const QString &command) {

    setCommand(command);

    CharacterStats stats = player()->stats();

    player()->send("\n" +
                   Util::colorize(QString("/============================== Overview ==================================\\\n"
                                          "| %1 |\n"
                                          "\\==========================================================================/\n")
                                  .arg(Util::center(QString("%1, a %2 %3 %4")
                                                    .arg(player()->name(), player()->gender(),
                                                         player()->race()->name().toLower(),
                                                         player()->characterClass()->name().toLower()), 72)), Green));
    player()->send(Util::colorize(QString(" /==== Attributes =====\\ \n"
                                          " | STR:  %1   DEX:  %2 | \n"
                                          " | VIT:  %3   END:  %4 | \n"
                                          " | INT:  %5   FAI:  %6 | \n"
                                          " \\=====================/ \n")
                                  .arg(QString::number(stats.strength).rightJustified(2),
                                       QString::number(stats.dexterity).rightJustified(2),
                                       QString::number(stats.vitality).rightJustified(2),
                                       QString::number(stats.endurance).rightJustified(2),
                                       QString::number(stats.intelligence).rightJustified(2),
                                       QString::number(stats.faith).rightJustified(2)), Olive) +
                   "\n");
}
