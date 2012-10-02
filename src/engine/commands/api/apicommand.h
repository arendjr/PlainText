#ifndef APICOMMAND_H
#define APICOMMAND_H

#include "admincommand.h"


class ApiCommand : public AdminCommand {

    Q_OBJECT

    public:
        ApiCommand(QObject *parent = 0);
        virtual ~ApiCommand();

        virtual void execute(Player *player, const QString &command);

        const QString &requestId() const { return m_requestId; }

    protected:
        void sendReply(const QVariant &variant);
        void sendError(int errorCode, const QString &errorMessage);

    private:
        QString m_requestId;
};

#endif // APICOMMAND_H
