#include "commandinterpreter.h"

#include <QCoreApplication>
#include <QRegExp>
#include <QStringList>

#include "engine/area.h"
#include "engine/character.h"
#include "engine/exit.h"
#include "engine/gameobjectptr.h"
#include "engine/realm.h"
#include "engine/util.h"


CommandInterpreter::CommandInterpreter(Character *character, QObject *parent) :
    QObject(parent),
    m_character(character) {
}

CommandInterpreter::~CommandInterpreter() {
}

void CommandInterpreter::execute(const QString &command) {

    QStringList words = command.trimmed().split(QRegExp("\\s+"));
    if (words[0] == "exit") {
        exit();
    } else if (words[0] == "go") {
        if (words.length() < 2) {
            m_character->send("Go where?");
        } else {
            goExit(words[1]);
        }
    } else if (words[0] == "say") {
        if (words.length() < 2) {
            m_character->send("Say what?");
        } else {
            say(Util::fromIndex(words, 1));
        }
    } else if (words[0] == "/me") {
        if (words.length() < 2) {
            m_character->send("C'mon, a little bit more creative...");
        } else {
            slashMe(Util::fromIndex(words, 1));
        }
    } else {
        if (m_character->isAdmin()) {
            if (words[0] == "get-prop") {
                if (words.length() < 3) {
                    m_character->send("Usage: get-prop <object-name> <property-name>");
                } else {
                    getProperty(localObjectByName(words[1]), words[2]);
                }
            } else if (words[0] == "set-prop") {
                if (words.length() < 4) {
                    m_character->send("Usage: set-prop <object-name> <property-name> <value>");
                } else {
                    setProperty(localObjectByName(words[1]), words[2], Util::fromIndex(words, 3));
                }
            } else if (words[0] == "add-exit") {
                if (words.length() < 3) {
                    m_character->send("Usage: add-exit <exit-name> <destination-area-id> [<hidden?>]");
                } else {
                    addExit(words[1], words[2], words.length() > 3 ? (words[3] == "true") : false);
                }
            } else if (words[0] == "remove-exit") {
                if (words.length() < 2) {
                    m_character->send("Usage: remove-exit <exit-name>");
                } else {
                    removeExit(words[1]);
                }
            } else if (words[0] == "stop-server") {
                qApp->quit();
            }
        }
    }
}

void CommandInterpreter::goExit(const QString &exitName) {

    Area *currentArea = m_character->currentArea().cast<Area *>();
    Q_ASSERT(currentArea);

    foreach (Exit exit, currentArea->exits()) {
        if (exit.name() == exitName) {
            leaveArea(m_character->currentArea(), exitName);
            enterArea(exit.destinationArea());
            return;
        }
    }

    m_character->send(QString("There's no exit named %1.").arg(exitName));
}

void CommandInterpreter::say(const QString &sentence) {

    GameObjectPtrList characters = m_character->currentArea().cast<Area *>()->presentCharacters();
    for (int i = 0; i < characters.length(); i++) {
        Character *character = characters[i].cast<Character *>();
        Q_ASSERT(character);

        character->send(QString("%1 said: %2").arg(m_character->name(), sentence));
    }
}

void CommandInterpreter::slashMe(const QString &sentence) {

    GameObjectPtrList characters = m_character->currentArea().cast<Area *>()->presentCharacters();
    for (int i = 0; i < characters.length(); i++) {
        Character *character = characters[i].cast<Character *>();
        Q_ASSERT(character);

        character->send(Util::colorize("%1 %2", Purple).arg(m_character->name(), sentence));
    }
}

void CommandInterpreter::enterArea(const GameObjectPtr &areaPtr) {

    m_character->setCurrentArea(areaPtr);

    Area *area = areaPtr.cast<Area *>();
    area->addPresentCharacter(m_character);

    if (!area->title().isEmpty()) {
        m_character->send(Util::colorize(area->title() + "\n\n", White));
    }

    m_character->send(area->description());

    GameObjectPtrList characters = area->presentCharacters();
    characters.removeOne(m_character);
    int length = characters.length();
    if (length > 0) {
        QStringList characterNames;
        for (int i = 0; i < length; i++) {
            Character *character = characters[i].cast<Character *>();
            Q_ASSERT(character);

            characterNames << character->name();

            character->send(QString("%1 arrived.").arg(m_character->name()));
        }
        m_character->send("\n\nYou see " + Util::joinFancy(characterNames) + ".");
    }

    ExitList exits = area->exits();
    length = exits.length();
    if (length > 0) {
        QStringList exitNames;
        for (int i = 0; i < length; i++) {
            exitNames << exits[i].name();
        }
        m_character->send("\n\nObvious exits: " + exitNames.join(", ") + ".");
    }
}

void CommandInterpreter::leaveArea(const GameObjectPtr &areaPtr, const QString &exitName) {

    Area *area = areaPtr.cast<Area *>();
    Q_ASSERT(area);
    area->removePresentCharacter(m_character);

    GameObjectPtrList characters = area->presentCharacters();
    foreach (GameObjectPtr characterPtr, characters) {
        Character *character = characterPtr.cast<Character *>();
        Q_ASSERT(character);

        if (exitName.isEmpty()) {
            character->send(QString("%1 left.").arg(m_character->name()));
        } else {
            character->send(QString("%1 left to the %2.").arg(m_character->name(), exitName));
        }
    }
}

const GameObjectPtr &CommandInterpreter::localObjectByName(const QString &objectName) const {

    if (m_character->isAdmin()) {
        if (objectName == "area") {
            return m_character->currentArea();
        }
    }

    return GameObjectPtr::null;
}

void CommandInterpreter::getProperty(const GameObjectPtr &object, const QString &propertyName) {

    if (propertyName == "id") {
        m_character->send(QString::number(object->id()));
    } else {
        QVariant value = object->property(propertyName.toAscii().constData());
        switch (value.type()) {
            case QVariant::Bool:
                m_character->send(value.toBool() ? "true" : "false");
                break;
            case QVariant::Int:
                m_character->send(QString::number(value.toInt()));
                break;
            case QVariant::String:
                m_character->send(value.toString());
                break;
            case QVariant::UserType:
                if (value.userType() == QMetaType::type("GameObjectPtr")) {
                    m_character->send(value.value<GameObjectPtr>().toString());
                    break;
                } else if (value.userType() == QMetaType::type("GameObjectPtrList")) {
                    QStringList strings;
                    foreach (GameObjectPtr pointer, value.value<GameObjectPtrList>()) {
                        strings << pointer.toString();
                    }
                    m_character->send("[ " + strings.join(", ") + " ]");
                    break;
                } else if (value.userType() == QMetaType::type("Exit")) {
                    m_character->send(value.value<Exit>().toString());
                    break;
                } else if (value.userType() == QMetaType::type("ExitList")) {
                    QStringList strings;
                    foreach (Exit exit, value.value<ExitList>()) {
                        strings << exit.toString();
                    }
                    m_character->send("[ " + strings.join(", ") + " ]");
                    break;
                }
            default:
                m_character->send("Cannot show property of unknown type.");
        }
    }
}

void CommandInterpreter::setProperty(const GameObjectPtr &object, const QString &propertyName,
                                     const QString &value) {

    QVariant variant = object->property(propertyName.toAscii().constData());
    switch (variant.type()) {
        case QVariant::Bool:
            variant = (value == "true");
            break;
        case QVariant::Int:
            variant = value.toInt();
            break;
        case QVariant::String:
            variant = value;
            break;
        case QVariant::UserType:
            if (variant.userType() == QMetaType::type("GameObjectPtr")) {
                variant = QVariant::fromValue(GameObjectPtr::fromString(value));
                break;
            } else if (variant.userType() == QMetaType::type("GameObjectPtrList")) {
                try {
                    GameObjectPtrList pointerList;
                    foreach (QString string, value.split(' ')) {
                        pointerList << GameObjectPtr::fromString(string);
                    }
                    variant = QVariant::fromValue(pointerList);
                } catch (BadGameObjectException exception) {
                    m_character->send(exception.what());
                }
                break;
            }
        default:
            m_character->send("Setting this property is not supported.");
    }
    object->setProperty(propertyName.toAscii().constData(), variant);

    if (m_character->currentArea() == object) {
        enterArea(m_character->currentArea());
    }
}

void CommandInterpreter::addExit(const QString &exitName, const QString &destinationAreaId, bool hidden) {

    GameObject *destinationArea;
    if (destinationAreaId == "new") {
        destinationArea = GameObject::createByObjectType("area");
    } else {
        destinationArea = Realm::instance()->getObject("area", destinationAreaId.toInt());
        if (!destinationArea) {
            m_character->send(QString("No area with ID %1.").arg(destinationAreaId));
            return;
        }
    }

    Exit exit;
    exit.setName(exitName);
    exit.setDestinationArea(destinationArea);
    exit.setHidden(hidden);
    m_character->currentArea().cast<Area *>()->addExit(exit);

    enterArea(m_character->currentArea());
}

void CommandInterpreter::removeExit(const QString &exitName) {

    Area *currentArea = m_character->currentArea().cast<Area *>();
    Q_ASSERT(currentArea);

    foreach (Exit exit, currentArea->exits()) {
        if (exit.name() == exitName) {
            currentArea->removeExit(exit);

            enterArea(m_character->currentArea());
            return;
        }
    }

    m_character->send(QString("There's no exit named %1.").arg(exitName));
}
