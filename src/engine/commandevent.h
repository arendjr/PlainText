#ifndef COMMANDEVENT_H
#define COMMANDEVENT_H

#include "event.h"


class CommandInterpreter;

class CommandEvent : public Event {

    public:
        CommandEvent(CommandInterpreter *interpreter, const QString &command);
        virtual ~CommandEvent();

        virtual void process();

        virtual QString toString() const;

    private:
        CommandInterpreter *m_interpreter;
        QString m_command;
};

#endif // COMMANDEVENT_H
