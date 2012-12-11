#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"

#include <QString>


class Session;

class Player : public Character {

    Q_OBJECT

    public:
        Player(Realm *realm, uint id = 0, Options options = NoOptions);
        virtual ~Player();

        const QString &passwordSalt() const { return m_passwordSalt; }
        void setPasswordSalt(const QString &passwordSalt);
        Q_PROPERTY(QString passwordSalt READ passwordSalt WRITE setPasswordSalt)

        const QString &passwordHash() const { return m_passwordHash; }
        void setPasswordHash(const QString &passwordHash);
        Q_PROPERTY(QString passwordHash READ passwordHash WRITE setPasswordHash)

        Q_INVOKABLE void setPassword(const QString &password);
        Q_INVOKABLE bool matchesPassword(const QString &password) const;

        bool isAdmin() const { return m_admin; }
        void setAdmin(bool admin);
        Q_PROPERTY(bool admin READ isAdmin WRITE setAdmin)

        Session *session() const { return m_session; }
        void setSession(Session *session);
        Q_INVOKABLE bool isOnline() const;

        virtual void send(const QString &message, int color = Silver) const;

        Q_INVOKABLE void sendSellableItemsList(const GameObjectPtrList &items);

        Q_INVOKABLE void execute(const QString &command);

        Q_INVOKABLE void quit();

        virtual void invokeTimer(int timerId);

    protected:
        virtual void changeName(const QString &name);

    private:
        QString m_passwordSalt;
        QString m_passwordHash;

        int m_regenerationIntervalId;

        bool m_admin;

        Session *m_session;
};

#endif // PLAYER_H
