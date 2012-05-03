#include "commandinterpreter.h"

#include <QCoreApplication>
#include <QRegExp>
#include <QStringList>

#include "engine/area.h"
#include "engine/character.h"
#include "engine/exit.h"
#include "engine/gameobjectptr.h"


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
            write("Go where?");
        } else {
            goExit(words[1]);
        }
    } else {
        if (m_character->isAdmin()) {
            if (words[0] == "stop-server") {
                qApp->quit();
            }
        }
    }
}

void CommandInterpreter::goExit(const QString &exitName) {

    foreach (Exit exit, m_character->currentArea().cast<Area>()->exits()) {
        if (exit.name() == exitName) {
            enterArea(exit.destinationArea());
            return;
        }
    }

    write(QString("There's no exit named %1.").arg(exitName));
}

void CommandInterpreter::enterArea(const GameObjectPtr &areaPtr) {

    if (m_character->currentArea() != areaPtr) {
        m_character->currentArea().cast<Area>()->removePresentCharacter(m_character);
        m_character->setCurrentArea(areaPtr);
    }

    Area *area = areaPtr.cast<Area>();
    area->addPresentCharacter(m_character);

    if (!area->title().isEmpty()) {
        write(area->title() + "\n\n");
    }

    write(area->description());

    GameObjectPtrList characters = area->presentCharacters();
    characters.removeOne(m_character);
    int length = characters.length();
    if (length > 0) {
        write("\n\nYou see ");
        for (int i = 0; i < length; i++) {
            write(characters[i].cast<Character>()->name());
            if (i < length - 2) {
                write(", ");
            } else if (i == length - 2) {
                write(" and ");
            }
        }
        write(".");
    }
}
