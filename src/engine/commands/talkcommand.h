#ifndef TALKCOMMAND_H
#define TALKCOMMAND_H

#include "command.h"


class TalkCommand : public Command {

    Q_OBJECT

    public:
        explicit TalkCommand(Player *player, QObject *parent = 0);
        virtual ~TalkCommand();

        virtual void execute(const QString &command);
};

#endif // TALKCOMMAND_H
