#ifndef COMMANDEVENT_H
#define COMMANDEVENT_H

#include "event.h"


class Player;

class CommandEvent : public Event {

    public:
        CommandEvent(Player *player, const QString &command);
        virtual ~CommandEvent();

        virtual void process();

        virtual QString toString() const;

    private:
        Player *m_player;
        QString m_command;
};

#endif // COMMANDEVENT_H
