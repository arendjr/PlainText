#include "commandinterpreter.h"

#include <QRegExp>
#include <QStringList>

#include "area.h"
#include "exit.h"
#include "gameexception.h"
#include "gameobjectptr.h"
#include "logutil.h"
#include "player.h"
#include "realm.h"
#include "util.h"
#include "commands/buycommand.h"
#include "commands/closecommand.h"
#include "commands/descriptioncommand.h"
#include "commands/disbandcommand.h"
#include "commands/drinkcommand.h"
#include "commands/dropcommand.h"
#include "commands/eatcommand.h"
#include "commands/equipmentcommand.h"
#include "commands/followcommand.h"
#include "commands/getcommand.h"
#include "commands/givecommand.h"
#include "commands/gocommand.h"
#include "commands/groupcommand.h"
#include "commands/gtalkcommand.h"
#include "commands/helpcommand.h"
#include "commands/inventorycommand.h"
#include "commands/killcommand.h"
#include "commands/lookcommand.h"
#include "commands/losecommand.h"
#include "commands/opencommand.h"
#include "commands/quitcommand.h"
#include "commands/saycommand.h"
#include "commands/searchcommand.h"
#include "commands/shoutcommand.h"
#include "commands/slashmecommand.h"
#include "commands/statscommand.h"
#include "commands/talkcommand.h"
#include "commands/tellcommand.h"
#include "commands/usecommand.h"
#include "commands/whocommand.h"
#include "commands/admin/addcharactercommand.h"
#include "commands/admin/addexitcommand.h"
#include "commands/admin/additemcommand.h"
#include "commands/admin/addshieldcommand.h"
#include "commands/admin/addweaponcommand.h"
#include "commands/admin/copyitemcommand.h"
#include "commands/admin/copytriggerscommand.h"
#include "commands/admin/execscriptcommand.h"
#include "commands/admin/getpropcommand.h"
#include "commands/admin/gettriggercommand.h"
#include "commands/admin/listmethodscommand.h"
#include "commands/admin/listpropscommand.h"
#include "commands/admin/removeexitcommand.h"
#include "commands/admin/removeitemcommand.h"
#include "commands/admin/setclasscommand.h"
#include "commands/admin/setpropcommand.h"
#include "commands/admin/setracecommand.h"
#include "commands/admin/settriggercommand.h"
#include "commands/admin/stopservercommand.h"
#include "commands/admin/unsettriggercommand.h"
#include "commands/api/areaslistcommand.h"
#include "commands/api/datagetcommand.h"
#include "commands/api/datasetcommand.h"
#include "commands/api/exitslistcommand.h"
#include "commands/api/logretrievecommand.h"
#include "commands/api/propertygetcommand.h"
#include "commands/api/triggergetcommand.h"
#include "commands/api/triggersetcommand.h"
#include "commands/api/triggerslistcommand.h"


CommandInterpreter::CommandInterpreter(Player *player) :
    QObject(),
    m_player(player) {

    Command *get = new GetCommand(player, this);
    Command *go = new GoCommand(player, this);
    Command *kill = new KillCommand(player, this);
    Command *look = new LookCommand(player, this);
    Command *quit = new QuitCommand(player, this);

    m_commands.insert("attack", kill);
    m_commands.insert("buy", new BuyCommand(player, this));
    m_commands.insert("close", new CloseCommand(player, this));
    m_commands.insert("description", new DescriptionCommand(player, this));
    m_commands.insert("disband", new DisbandCommand(player, this));
    m_commands.insert("drink", new DrinkCommand(player, this));
    m_commands.insert("drop", new DropCommand(player, this));
    m_commands.insert("eat", new EatCommand(player, this));
    m_commands.insert("enter", go);
    m_commands.insert("equipment", new EquipmentCommand(player, this));
    m_commands.insert("follow", new FollowCommand(player, this));
    m_commands.insert("get", get);
    m_commands.insert("give", new GiveCommand(player, this));
    m_commands.insert("go", go);
    m_commands.insert("goodbye", quit);
    m_commands.insert("group", new GroupCommand(player, this));
    m_commands.insert("gtalk", new GtalkCommand(player, this));
    m_commands.insert("help", new HelpCommand(player, m_commands, this));
    m_commands.insert("inventory", new InventoryCommand(player, this));
    m_commands.insert("kill", kill);
    m_commands.insert("l", look);
    m_commands.insert("look", look);
    m_commands.insert("lose", new LoseCommand(player, this));
    m_commands.insert("open", new OpenCommand(player, this));
    m_commands.insert("quit", quit);
    m_commands.insert("say", new SayCommand(player, this));
    m_commands.insert("search", new SearchCommand(player, this));
    m_commands.insert("shout", new ShoutCommand(player, this));
    m_commands.insert("stats", new StatsCommand(player, this));
    m_commands.insert("take", get);
    m_commands.insert("talk", new TalkCommand(player, this));
    m_commands.insert("tell", new TellCommand(player, this));
    m_commands.insert("use", new UseCommand(player, this));
    m_commands.insert("who", new WhoCommand(player, this));
    m_commands.insert("/me", new SlashMeCommand(player, this));

    if (m_player->isAdmin()) {
        m_commands.insert("add-character", new AddCharacterCommand(player, this));
        m_commands.insert("add-exit", new AddExitCommand(player, this));
        m_commands.insert("add-item", new AddItemCommand(player, this));
        m_commands.insert("add-shield", new AddShieldCommand(player, this));
        m_commands.insert("add-weapon", new AddWeaponCommand(player, this));
        m_commands.insert("copy-item", new CopyItemCommand(player, this));
        m_commands.insert("copy-triggers", new CopyTriggersCommand(player, this));
        m_commands.insert("exec-script", new ExecScriptCommand(player, this));
        m_commands.insert("get-prop", new GetPropCommand(player, this));
        m_commands.insert("get-trigger", new GetTriggerCommand(player, this));
        m_commands.insert("list-methods", new ListMethodsCommand(player, this));
        m_commands.insert("list-props", new ListPropsCommand(player, this));
        m_commands.insert("remove-exit", new RemoveExitCommand(player, this));
        m_commands.insert("remove-item", new RemoveItemCommand(player, this));
        m_commands.insert("set-class", new SetClassCommand(player, this));
        m_commands.insert("set-prop", new SetPropCommand(player, this));
        m_commands.insert("set-race", new SetRaceCommand(player, this));
        m_commands.insert("set-trigger", new SetTriggerCommand(player, this));
        m_commands.insert("stop-server", new StopServerCommand(player, this));
        m_commands.insert("unset-trigger", new UnsetTriggerCommand(player, this));

        m_commands.insert("api-areas-list", new AreasListCommand(player, this));
        m_commands.insert("api-data-get", new DataGetCommand(player, this));
        m_commands.insert("api-data-set", new DataSetCommand(player, this));
        m_commands.insert("api-exits-list", new ExitsListCommand(player, this));
        m_commands.insert("api-log-retrieve", new LogRetrieveCommand(player, this));
        m_commands.insert("api-property-get", new PropertyGetCommand(player, this));
        m_commands.insert("api-trigger-get", new TriggerGetCommand(player, this));
        m_commands.insert("api-trigger-set", new TriggerSetCommand(player, this));
        m_commands.insert("api-triggers-list", new TriggersListCommand(player, this));
    }

    connect(quit, SIGNAL(quit()), this, SIGNAL(quit()));

    LogUtil::logCommand(m_player->name(), "(signed in)");
}

CommandInterpreter::~CommandInterpreter() {

    LogUtil::logCommand(m_player->name(), "(signed out)");
}

void CommandInterpreter::execute(const QString &command) {

    static QRegExp whitespace("\\s+");

    try {
        LogUtil::logCommand(m_player->name(), command);

        QStringList words = command.trimmed().split(whitespace);
        QString commandName = words[0].toLower();
        if (commandName.isEmpty()) {
            return;
        }

        if (Util::isDirectionAbbreviation(commandName)) {
            words[0] = Util::direction(commandName);
            commandName = words[0];
        }
        if (Util::isDirection(commandName) || commandName == "out") {
            words.prepend("go");
            m_commands["go"]->execute(words.join(" "));
            return;
        }

        if (m_commands.contains(commandName)) {
            m_commands[commandName]->execute(command);
            return;
        }

        QList<Command *> commands;
        for (const QString &key : m_commands.keys()) {
            if (key.startsWith(commandName)) {
                commands << m_commands[key];
            }
        }

        if (commands.length() == 1) {
            commands[0]->execute(command);
        } else if (commands.length() > 1) {
            m_player->send("Command is not unique.");
        } else {
            m_player->send(QString("Command \"%1\" does not exist.").arg(words[0]));
        }
    } catch (GameException &exception) {
        m_player->send(QString("Executing the command gave an exception: "
                               "%1").arg(exception.what()));
        if (!m_player->isAdmin()) {
            m_player->send("This is not good. You may want to contact a game admin about this.");
        }
    } catch (...) {
        m_player->send("Executing the command gave an unknown exception.");
        if (!m_player->isAdmin()) {
            m_player->send("This is not good. You may want to contact a game admin about this.");
        }
    }
}
