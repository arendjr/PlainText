#ifndef CHARACTER_H
#define CHARACTER_H

#include "gameobject.h"
#include "gameobjectptr.h"

#include <QString>


class Session;

class Character : public GameObject {

    Q_OBJECT

    public:
        explicit Character(uint id, Options options = NoOptions);
        virtual ~Character();

        virtual void setName(const QString &name);

        const QString &passwordHash() const { return m_passwordHash; }
        virtual void setPasswordHash(const QString &passwordHash);
        Q_PROPERTY(QString passwordHash READ passwordHash WRITE setPasswordHash);

        const GameObjectPtr &currentArea() const { return m_currentArea; }
        virtual void setCurrentArea(const GameObjectPtr &currentArea);
        Q_PROPERTY(GameObjectPtr currentArea READ currentArea WRITE setCurrentArea);

        const GameObjectPtrList &inventory() const { return m_inventory; }
        virtual void addInventoryItem(const GameObjectPtr &item);
        virtual void removeInventoryItem(const GameObjectPtr &item);
        virtual void setInventory(const GameObjectPtrList &inventory);
        Q_PROPERTY(GameObjectPtrList inventory READ inventory WRITE setInventory);

        bool isAdmin() const { return m_admin; }
        virtual void setAdmin(bool admin);
        Q_PROPERTY(bool admin READ isAdmin WRITE setAdmin);

        Session *session() const { return m_session; }
        void setSession(Session *session);

        void send(QString data);

        void enter(const GameObjectPtr &area);
        void leave(const GameObjectPtr &area, const QString &exitName = QString::null);

    signals:
        void write(const QString &data);

    private:
        QString m_passwordHash;

        GameObjectPtr m_currentArea;

        GameObjectPtrList m_inventory;

        bool m_admin;

        Session *m_session;
};

#endif // CHARACTER_H
