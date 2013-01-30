#ifndef SPEECHEVENT_H
#define SPEECHEVENT_H

#include "character.h"
#include "gameobjectptr.h"
#include "soundevent.h"


class Character;

class SpeechEvent : public SoundEvent {

    Q_OBJECT

    public:
        SpeechEvent(Room *origin, double strength);
        virtual ~SpeechEvent();

        GameObjectPtr speaker() { return GameObjectPtr(m_speaker); }
        void setSpeaker(const GameObjectPtr &speaker);
        Q_PROPERTY(GameObjectPtr speaker READ speaker WRITE setSpeaker)

        const QString &message() const { return m_message; }
        void setMessage(const QString &message);
        Q_PROPERTY(QString message READ message WRITE setMessage)

        const GameObjectPtr &target() { return m_target; }
        void setTarget(const GameObjectPtr &target);
        Q_PROPERTY(GameObjectPtr target READ target WRITE setTarget)

        Q_INVOKABLE virtual QString descriptionForStrengthAndCharacterInRoom(double strength,
                                                                             Character *character,
                                                                             Room *room) const;

    private:
        Character *m_speaker;
        QString m_message;
        GameObjectPtr m_target;
        bool m_isShout;
};

#endif // SPEECHEVENT_H
