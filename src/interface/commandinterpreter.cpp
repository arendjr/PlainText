#include "commandinterpreter.h"

#include <QCoreApplication>
#include <QRegExp>
#include <QStringList>

#include "engine/area.h"
#include "engine/character.h"
#include "engine/exit.h"
#include "engine/gameobjectptr.h"
#include "engine/realm.h"


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
    } else {
        if (m_character->isAdmin()) {
            if (words[0] == "add-exit") {
                if (words.length() < 3) {
                    m_character->send("Usage: add-exit <exit-name> <destination-area-id> [<hidden?>]");
                } else {
                    addExit(words[1], words[2].toInt(), words.length() > 3 ? (words[3] == "true") : false);
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

    Area *currentArea = m_character->currentArea().cast<Area>();
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

void CommandInterpreter::enterArea(const GameObjectPtr &areaPtr) {

    m_character->setCurrentArea(areaPtr);

    Area *area = areaPtr.cast<Area>();
    area->addPresentCharacter(m_character);

    if (!area->title().isEmpty()) {
        m_character->send(area->title() + "\n\n");
    }

    m_character->send(area->description());

    GameObjectPtrList characters = area->presentCharacters();
    characters.removeOne(m_character);
    int length = characters.length();
    if (length > 0) {
        QString sentence = "\n\nYou see ";
        for (int i = 0; i < length; i++) {
            Character *character = characters[i].cast<Character>();
            Q_ASSERT(character);

            sentence += character->name();
            if (i < length - 2) {
                sentence += ", ";
            } else if (i == length - 2) {
                sentence += " and ";
            }

            character->send(QString("%1 arrived.").arg(m_character->name()));
        }
        m_character->send(sentence + ".");
    }

    ExitList exits = area->exits();
    length = exits.length();
    if (length > 0) {
        QString sentence = "\n\nObvious exits: ";
        for (int i = 0; i < length; i++) {
            Exit exit = exits[i];

            sentence += exit.name();
            if (i < length - 1) {
                sentence += ", ";
            }
        }
        m_character->send(sentence + ".");
    }
}

void CommandInterpreter::leaveArea(const GameObjectPtr &areaPtr, const QString &exitName) {

    Area *area = areaPtr.cast<Area>();
    Q_ASSERT(area);
    area->removePresentCharacter(m_character);

    GameObjectPtrList characters = area->presentCharacters();
    foreach (GameObjectPtr characterPtr, characters) {
        Character *character = characterPtr.cast<Character>();
        Q_ASSERT(character);

        if (exitName.isEmpty()) {
            character->send(QString("%1 left.").arg(m_character->name()));
        } else {
            character->send(QString("%1 left to the %2.").arg(m_character->name(), exitName));
        }
    }
}

void CommandInterpreter::addExit(const QString &exitName, uint destinationAreaId, bool hidden) {

    GameObject *destinationArea = Realm::instance()->getObject("area", destinationAreaId);
    if (!destinationArea) {
        m_character->send(QString("No area with ID %1.").arg(destinationAreaId));
        return;
    }

    Exit exit;
    exit.setName(exitName);
    exit.setDestinationArea(destinationArea);
    exit.setHidden(hidden);
    m_character->currentArea().cast<Area>()->addExit(exit);

    enterArea(m_character->currentArea());
}

void CommandInterpreter::removeExit(const QString &exitName) {

    Area *currentArea = m_character->currentArea().cast<Area>();
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
