#ifndef APICOMMAND_H
#define APICOMMAND_H

#include "admincommand.h"


class ApiCommand : public AdminCommand {

    Q_OBJECT

    public:
        ApiCommand(QObject *parent = 0);
        virtual ~ApiCommand();

        const QString &requestId() const { return m_requestId; }

    protected:
        virtual void prepareExecute(Character *character, const QString &command);

        void sendReply(const QVariant &variant);
        void sendError(int errorCode, const QString &errorMessage);

    private:
        QString m_requestId;
};

#endif // APICOMMAND_H
