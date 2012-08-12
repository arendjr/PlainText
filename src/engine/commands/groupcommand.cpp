#include "groupcommand.h"

#include "engine/group.h"
#include "engine/util.h"


GroupCommand::GroupCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription("Show information about the group you are in.\n"
                   "\n"
                   "Example: group");
}

GroupCommand::~GroupCommand() {
}

void GroupCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();

    if (player()->group().isNull()) {
        send("You are not in a group.");
    } else {
        Group *group = player()->group().cast<Group *>();

        Character *leader = group->leader().cast<Character *>();
        Color hpColor = (leader->hp() < leader->maxHp() / 4 ? Maroon : Silver);
        Color mpColor = (leader->mp() < leader->maxMp() / 4 ? Maroon : Silver);
        send(QString("Leader:  %1  %2  %3").arg(leader->name().leftJustified(20))
             .arg(Util::colorize(QString("%1HP").arg(leader->hp()).rightJustified(5), hpColor))
             .arg(Util::colorize(QString("%1MP").arg(leader->mp()).rightJustified(5), mpColor)));

        for (const GameObjectPtr &memberPtr : group->members()) {
            Character *member = memberPtr.cast<Character *>();
            Color hpColor = (member->hp() < member->maxHp() / 4 ? Maroon : Silver);
            Color mpColor = (member->mp() < member->maxMp() / 4 ? Maroon : Silver);
            send(QString("Member:  %1  %2  %3").arg(member->name().leftJustified(20))
                 .arg(Util::colorize(QString("%1HP").arg(member->hp()).rightJustified(5), hpColor))
                 .arg(Util::colorize(QString("%1MP").arg(member->mp()).rightJustified(5),
                                     mpColor)));
        }
    }
}
