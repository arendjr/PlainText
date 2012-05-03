#ifndef CHARACTER_H
#define CHARACTER_H

#include "gameobject.h"
#include "gameobjectptr.h"

#include <QString>


class Session;

class Character : public GameObject {

    Q_OBJECT

    public:
        explicit Character(uint id, QObject *parent = 0);
        virtual ~Character();

        const QString &name() const { return m_name; }
        void setName(const QString &name);
        Q_PROPERTY(QString name READ name WRITE setName);

        const QString &passwordHash() const { return m_passwordHash; }
        void setPasswordHash(const QString &passwordHash);
        Q_PROPERTY(QString passwordHash READ passwordHash WRITE setPasswordHash);

        const GameObjectPtr &currentArea() const { return m_currentArea; }
        void setCurrentArea(const GameObjectPtr &currentArea);
        Q_PROPERTY(GameObjectPtr currentArea READ currentArea WRITE setCurrentArea);

        bool isAdmin() const { return m_admin; }
        void setAdmin(bool admin);
        Q_PROPERTY(bool admin READ isAdmin WRITE setAdmin);

        Session *session() const { return m_session; }
        void setSession(Session *session);

        void send(QString data);

    signals:
        void write(const QString &data);

    private:
        QString m_name;
        QString m_passwordHash;

        GameObjectPtr m_currentArea;

        bool m_admin;

        Session *m_session;
};

#endif // CHARACTER_H
