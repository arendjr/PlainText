#ifndef TALKCOMMAND_H
#define TALKCOMMAND_H

#include "command.h"


class TalkCommand : public Command {

    Q_OBJECT

    public:
        TalkCommand(QObject *parent = 0);
        virtual ~TalkCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // TALKCOMMAND_H
