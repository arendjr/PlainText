#include "movementvisualevent.h"

#include <QDebug>

#include "character.h"
#include "gameexception.h"
#include "point3d.h"
#include "portal.h"
#include "room.h"
#include "util.h"
#include "vector3d.h"


#define super VisualEvent

MovementVisualEvent::MovementVisualEvent(Room *origin, double strength) :
    super(GameEventType::MovementVisual, origin, strength),
    m_destination(nullptr) {
}

MovementVisualEvent::~MovementVisualEvent() {
}

void MovementVisualEvent::setDestination(const GameObjectPtr &destination) {

    try {
        m_destination = destination.cast<Room *>();

        addVisit(m_destination, strengthForRoom(originRoom()));
    } catch (GameException &exception) {
        qDebug() << "Exception in MovementVisualEvent::setDestination(): " << exception.what();
    }
}

void MovementVisualEvent::setMovement(const Vector3D &movement) {

    m_movement = movement;
}

void MovementVisualEvent::setDirection(const Vector3D &direction) {

    m_direction = direction;
}

void MovementVisualEvent::setVerb(const QString &simplePresent, const QString &continuous) {

    m_simplePresent = simplePresent;
    m_continuous = continuous;
}

QString MovementVisualEvent::descriptionForStrengthAndCharacterInRoom(double strength,
                                                                      Character *character,
                                                                      Room *room) const {

    Q_UNUSED(character)

    if (room == originRoom()) {
        QString characterName = Util::capitalize(description());
        for (const GameObjectPtr &portalPtr : room->portals()) {
            Portal *portal = portalPtr.cast<Portal *>();
            if (portal->room() == m_destination || portal->room2() == m_destination) {
                QString exitName = portal->nameFromRoom(room);
                if (Util::isDirection(exitName)) {
                    return QString("%1 %2 %3.").arg(characterName, m_simplePresent, exitName);
                } else if (exitName == "out") {
                    return QString("%1 %2 outside.");
                } else {
                    if (portal->canOpenFromRoom(room)) {
                        return QString("%1 %2 through the %3.").arg(characterName, m_simplePresent,
                                                                    exitName);
                    } else {
                        return QString("%1 %2 to the %3.").arg(characterName, m_simplePresent,
                                                               exitName);
                    }
                }
            }
        }

        return QString("%1 %2 %3.").arg(characterName, m_simplePresent,
                                        Util::directionForVector(m_direction));
    } else if (room == m_destination) {
        return QString("%1 %2 up to you.").arg(Util::capitalize(description()), m_simplePresent);
    }

    Vector3D vector = room->position() - originRoom()->position();
    int distance = vector.length();
    double angle = m_direction.angle(vector);

    QString prefix;
    if (distance > 100) {
        if (Util::randomInt(0, 5) < 3) {
            prefix = "In the distance," +
                     Util::randomAlternative(3, " you see", " you can see", "");
        } else {
            prefix = "From afar," + Util::randomAlternative(2, " you see", " you can see");
        }
    } else {
        prefix = "You see";
    }

    QString subject = strength > 0.9 ? description() :
                      strength > 0.8 ? distantDescription() :
                                       veryDistantDescription();

    QString helperVerb;
    if (prefix.endsWith(",")) {
        helperVerb = (m_simplePresent.endsWith("s") ? "is " : "are ");
    }

    QString direction;
    if (angle < TAU / 8) {
        direction = (distance > 100 ? "in your direction" : "toward you");
    } else if (angle >= TAU * 3 / 8 && distance <= 100) {
        direction = "away from you";
    } else {
        direction = Util::directionForVector(m_direction);
    }

    return QString("%1 %2 %3%4 %5.").arg(prefix, subject, helperVerb, m_continuous, direction);
}

bool MovementVisualEvent::isWithinSight(Room *targetRoom, Room *sourceRoom) {

    if (sourceRoom == originRoom() || sourceRoom == m_destination) {
        return true;
    }

    Vector3D sourceVector = (sourceRoom->position() - originRoom()->position()).normalized();
    Vector3D targetVector = (targetRoom->position() - sourceRoom->position()).normalized();
    if (sourceVector == targetVector) {
        return true;
    }

    if (~sourceRoom->flags() & RoomFlags::HasCeiling) {
        if (targetVector.z >= sourceVector.z) {
            return true;
        }
    }
    if (~sourceRoom->flags() & RoomFlags::HasFloor) {
        if (targetVector.z <= sourceVector.z) {
            return true;
        }
    }

    sourceVector = (sourceRoom->position() - m_destination->position()).normalized();
    if (sourceVector == targetVector) {
        return true;
    }

    if (~sourceRoom->flags() & RoomFlags::HasCeiling) {
        if (targetVector.z >= sourceVector.z) {
            return true;
        }
    }
    if (~sourceRoom->flags() & RoomFlags::HasFloor) {
        if (targetVector.z <= sourceVector.z) {
            return true;
        }
    }

    return false;
}
