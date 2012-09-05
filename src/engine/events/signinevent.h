#ifndef SIGNINEVENT_H
#define SIGNINEVENT_H

#include "event.h"


class Session;

class SignInEvent : public Event {

    public:
        SignInEvent(Session *session, const QString &input);
        virtual ~SignInEvent();

        virtual void process();

        virtual QString toString() const;

    private:
        Session *m_session;
        QString m_input;
};

#endif // SIGNINEVENT_H
