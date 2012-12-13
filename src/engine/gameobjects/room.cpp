#include "room.h"

#include <QDebug>

#include "exit.h"
#include "item.h"
#include "portal.h"
#include "util.h"


#define super GameObject

Room::Room(Realm *realm, uint id, Options options) :
    super(realm, GameObjectType::Room, id, (Options) (options | NeverDelete)),
    m_position(0, 0, 0),
    m_portals(8),
    m_flags(RoomFlags::NoFlags) {
}

Room::~Room() {
}

void Room::setArea(const GameObjectPtr &area) {

    if (m_area != area) {
        m_area = area;

        setModified();
    }
}

void Room::setPosition(const Point3D &position) {

    if (m_position != position) {
        m_position = position;

        setModified();
    }
}

void Room::addPortal(const GameObjectPtr &portal) {

    if (!m_portals.contains(portal)) {
        m_portals.append(portal);

        m_exits.clear();
        setModified();
    }
}

void Room::removePortal(const GameObjectPtr &portal) {

    if (m_portals.removeOne(portal)) {
        m_exits.clear();
        setModified();
    }
}

void Room::setPortals(const GameObjectPtrList &portals) {

    if (m_portals != portals) {
        m_portals = portals;

        m_exits.clear();
        setModified();
    }
}

const GameObjectPtrList &Room::exits() {

    if (m_exits.isEmpty()) {
        for (const GameObjectPtr &portalPtr : m_portals) {
            Portal *portal = portalPtr.cast<Portal *>();
            if (portal->flags() & PortalFlags::CanPassThrough ||
                (portal->flags() & PortalFlags::CanPassThroughIfOpen &&
                 portal->canOpenFromRoom(const_cast<Room *>(this)))) {
                Exit *exit = new Exit(realm());
                if (this == portal->room().cast<Room *>()) {
                    exit->setName(portal->name());
                    exit->setDescription(portal->description());
                    exit->setDestination(portal->room2());
                    exit->setDoor(portal->flags() & PortalFlags::CanOpenFromSide1);
                    exit->setHidden(portal->flags() & PortalFlags::IsHiddenFromSide1);
                    exit->setTriggers(portal->triggers());
                } else {
                    exit->setName(portal->name2());
                    exit->setDescription(portal->description2());
                    exit->setDestination(portal->room());
                    exit->setDoor(portal->flags() & PortalFlags::CanOpenFromSide2);
                    exit->setHidden(portal->flags() & PortalFlags::IsHiddenFromSide2);
                    exit->setTriggers(portal->triggers());
                }
                m_exits.append(exit);
            }
        }
    }
    return m_exits;
}

void Room::addCharacter(const GameObjectPtr &character) {

    if (!m_characters.contains(character)) {
        m_characters.append(character);
    }
}

void Room::removeCharacter(const GameObjectPtr &character) {

    m_characters.removeOne(character);
}

void Room::setCharacters(const GameObjectPtrList &characters) {

    if (m_characters != characters) {
        m_characters = characters;
    }
}

void Room::addItem(const GameObjectPtr &item) {

    if (!m_items.contains(item)) {
        m_items.append(item);

        setModified();
    }
}

void Room::removeItem(const GameObjectPtr &item) {

    if (m_items.removeOne(item)) {
        setModified();
    }
}

void Room::setItems(const GameObjectPtrList &items) {

    if (m_items != items) {
        m_items = items;

        setModified();
    }
}

void Room::setFlags(RoomFlags flags) {

    if (m_flags != flags) {
        m_flags = flags;

        setModified();
    }
}

void Room::setEventMultipliers(const GameEventMultiplierMap &multipliers) {

    if (m_eventMultipliers != multipliers) {
        m_eventMultipliers = multipliers;

        setModified();
    }
}

QString Room::lookAtBy(GameObject *character) {

    try {
        QString text;

        if (!name().isEmpty()) {
            text += "\n" + Util::colorize(name(), Teal) + "\n\n";
        }

        text += description() + "\n";

        if (!portals().isEmpty()) {
            QStringList exitNames;
            GameObjectPtr room(this);
            for (const GameObjectPtr &portalPtr : portals()) {
                Portal *portal = portalPtr.cast<Portal *>();

                if (portal->isHiddenFromRoom(room)) {
                    continue;
                }

                exitNames.append(portal->nameFromRoom(room));
            }
            exitNames = Util::sortExitNames(exitNames);
            text += Util::colorize("Obvious exits: " + exitNames.join(", ") + ".", Green) + "\n";
        }

        GameObjectPtrList others = characters();
        others.removeOne(character);
        if (!others.isEmpty()) {
            text += QString("You see %1.\n").arg(others.joinFancy());
        }

        if (!items().isEmpty()) {
            text += QString("You see %1.\n").arg(items().joinFancy());
        }

        return text;
    } catch (GameException &exception) {
        qDebug() << "Exception in Room::lookAtBy(): " << exception.what();
        return QString();
    }
}
