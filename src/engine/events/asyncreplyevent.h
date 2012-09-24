#ifndef ASYNCREPLYEVENT_H
#define ASYNCREPLYEVENT_H

#include "event.h"


class Player;

class AsyncReplyEvent : public Event {

    public:
        AsyncReplyEvent(Player *recipient, const QString &reply);
        virtual ~AsyncReplyEvent();

        virtual void process();

        virtual QString toString() const;

    private:
        Player *m_recipient;
        QString m_reply;
};

#endif // ASYNCREPLYEVENT_H
