#include "speechevent.h"

#include "character.h"
#include "room.h"
#include "util.h"


#define super SoundEvent

SpeechEvent::SpeechEvent(Character *speaker, const QString &message,
                         Room *origin, double strength) :
    super(origin, strength),
    m_speaker(speaker),
    m_message(message),
    m_isShout(strength > 1.0) {
}

SpeechEvent::~SpeechEvent() {
}

QString SpeechEvent::descriptionForStrengthInRoom(double strength, Room *room) const {

    if (strength >= 0.8) {
        QString description;
        if (m_isShout) {
            description = QString(".!?").contains(m_message.right(1)) ?
                          "%1 shouts, \"%2\"" : "%1 shouts, \"%2!\"";
        } else if (m_message.endsWith("?")) {
            description = "%1 asks, \"%2\"";
        } else {
            description = (m_message.endsWith(".") || m_message.endsWith("!")) ?
                          "%1 says, \"%2\"" : "%1 says, \"%2.\"";
        }
        return description.arg(m_speaker->definiteName(originRoom()->characters(), Capitalized),
                               Util::capitalize(m_message));
    } else if (strength >= 0.5) {
        QString garbledMessage;
        for (const QChar &character : m_message) {
            if (character.isLetterOrNumber() || character == ' ') {
                if (qrand() % 100 < 150.0 * (strength - 0.2) || character == ' ') {
                    garbledMessage.append(character);
                } else {
                    garbledMessage.append(".");
                }
            }
        }
        QString direction = Util::directionForVector(originRoom()->position() - room->position());
        return QString("You hear a %1 %2 %3, \"%4\"")
               .arg(m_speaker->gender() == "male" ? "man" : "woman",
                    direction == "up" ? "above" :
                    (direction == "down" ? "below" :
                     QString("in the " + direction)),
                    m_isShout ? "shouting" : "saying",
                    garbledMessage);
    } else if (strength >= (m_speaker->gender() == "male" ? 0.4 : 0.3)) {
        QString direction = Util::directionForVector(originRoom()->position() - room->position());
        if (direction == "up") {
            return QString("You hear a distant %1 from above.").arg(m_isShout ? "shout" : "mutter");
        } else if (direction == "down") {
            return QString("You hear a distant %1 from below.").arg(m_isShout ? "shout" : "mutter");
        } else {
            return QString("You hear a distant %1 from the %2.").arg(m_isShout ? "shout" : "mutter",
                                                                     direction);
        }
    } else {
        return QString("You hear a distant %1.").arg(m_isShout ? "shout" : "mutter");
    }
}
