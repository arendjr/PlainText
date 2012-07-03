#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"

#include <QString>


class Session;

class Player : public Character {

    Q_OBJECT

    public:
        Player(Realm *realm, uint id, Options options = NoOptions);
        virtual ~Player();

        virtual void setName(const QString &name);

        const QString &passwordSalt() const { return m_passwordSalt; }
        virtual void setPasswordSalt(const QString &passwordSalt);
        Q_PROPERTY(QString passwordSalt READ passwordSalt WRITE setPasswordSalt)

        const QString &passwordHash() const { return m_passwordHash; }
        virtual void setPasswordHash(const QString &passwordHash);
        Q_PROPERTY(QString passwordHash READ passwordHash WRITE setPasswordHash)

        bool isAdmin() const { return m_admin; }
        virtual void setAdmin(bool admin);
        Q_PROPERTY(bool admin READ isAdmin WRITE setAdmin)

        Session *session() const { return m_session; }
        void setSession(Session *session);

        virtual void send(const QString &message, Color color = Silver) const;
        Q_INVOKABLE void sendSellableItemsList(const GameObjectPtrList &items);

        virtual void enter(const GameObjectPtr &area);
        virtual void leave(const GameObjectPtr &area, const QString &exitName = QString());

        virtual void look();

        virtual void die(const GameObjectPtr &attacker);

        virtual void timerEvent(int timerId);

    signals:
        void write(const QString &data) const;

    private:
        QString m_passwordSalt;
        QString m_passwordHash;

        int m_regenerationInterval;

        bool m_admin;

        Session *m_session;
};

#endif // PLAYER_H
