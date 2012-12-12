#include "reloadscriptscommand.h"

#include "realm.h"
#include "scriptengine.h"


#define super AdminCommand

ReloadScriptsCommand::ReloadScriptsCommand(QObject *parent) :
    super(parent) {

    setDescription("Reloads all JavaScript files.\n"
                   "\n"
                   "Usage: reload-scripts");
}

ReloadScriptsCommand::~ReloadScriptsCommand() {
}

void ReloadScriptsCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObject::clearPrototypeMap();

    realm()->scriptEngine()->loadScripts();

    send("Scripts reloaded.");
}
