#ifndef SCRIPTCOMMAND_H
#define SCRIPTCOMMAND_H

#include "command.h"

#include <QScriptValue>


class ScriptCommand : public Command {

    Q_OBJECT

    public:
        ScriptCommand(const QScriptValue &object, QObject *parent = 0);
        virtual ~ScriptCommand();

        virtual void execute(Character *character, const QString &command);

    private:
        QScriptValue m_scriptCommand;
};

#endif // SCRIPTCOMMAND_H
