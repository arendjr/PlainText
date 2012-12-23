#ifndef COMMANDEVENT_H
#define COMMANDEVENT_H

#include "event.h"


class Character;

class CommandEvent : public Event {

    public:
        CommandEvent(Character *player, const QString &command);
        virtual ~CommandEvent();

        virtual void process();

        virtual QString toString() const;

    private:
        Character *m_player;
        QString m_command;
};

#endif // COMMANDEVENT_H
