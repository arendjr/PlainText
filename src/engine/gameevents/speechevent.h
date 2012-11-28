#ifndef SPEECHEVENT_H
#define SPEECHEVENT_H

#include "soundevent.h"


class Character;

class SpeechEvent : public SoundEvent {

    Q_OBJECT

    public:
        SpeechEvent(Character *speaker, const QString &message, Room *origin, double strength,
                    const GameObjectPtr &target = GameObjectPtr());
        virtual ~SpeechEvent();

        Q_INVOKABLE virtual QString descriptionForStrengthInRoom(double strength, Room *room) const;

    private:
        Character *m_speaker;
        QString m_message;
        bool m_isShout;
        GameObjectPtr m_target;
};

#endif // SPEECHEVENT_H
