#ifndef APICOMMAND_H
#define APICOMMAND_H

#include "admincommand.h"


class ApiCommand : public AdminCommand {

    Q_OBJECT

    public:
        ApiCommand(Player *player, QObject *parent = 0);
        virtual ~ApiCommand();

        virtual void execute(const QString &command);

    protected:
        void sendReply(const QVariant &variant);
        void sendError(int errorCode, const QString &errorMessage);

    private:
        QString m_requestId;
};

#endif // APICOMMAND_H
