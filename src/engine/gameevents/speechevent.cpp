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
        QStringList garbledWords;
        QString word;
        int numHeardWords = 0;
        for (const QChar &character : m_message) {
            if (character == ' ') {
                if (qrand() % 100 < 150.0 * (strength - 0.2)) {
                    garbledWords.append(word);
                    numHeardWords++;
                } else {
                    garbledWords.append(QString(word.length(), '.'));
                }
                word.clear();
            } else if (character.isLetterOrNumber()) {
                word.append(character);
            }
        }
        if (qrand() % 100 < 150.0 * (strength - 0.2)) {
            garbledWords.append(word);
            numHeardWords++;
        } else {
            garbledWords.append(QString(word.length(), '.'));
        }
        QString garbledMessage;
        if (numHeardWords == 0) {
            garbledMessage = " something, but you cannot make out a word of it.";
        } else {
            garbledMessage = QString(", \"%1\"").arg(garbledWords.join(" "));
        }

        QString direction = Util::directionForVector(originRoom()->position() - room->position());
        return QString("You hear a " + QString(m_speaker->gender() == "male" ? "man " : "woman ") +
                       (direction == "up" ? "from above " : (direction == "down" ? "from below " :
                                                            QString("to the " + direction + " "))) +
                       (m_isShout ? "shouting" : "saying") +
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
