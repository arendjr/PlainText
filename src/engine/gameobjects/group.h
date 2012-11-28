#ifndef GROUP_H
#define GROUP_H

#include "gameobject.h"
#include "gameobjectptr.h"


class Group : public GameObject {

    Q_OBJECT

    public:
        Group(Realm *realm, uint id = 0, Options options = NoOptions);
        virtual ~Group();

        const GameObjectPtr &leader() const { return m_leader; }
        void setLeader(const GameObjectPtr &leader);
        Q_PROPERTY(GameObjectPtr leader READ leader WRITE setLeader)

        const GameObjectPtrList &members() const { return m_members; }
        Q_INVOKABLE void addMember(const GameObjectPtr &member);
        Q_INVOKABLE void removeMember(const GameObjectPtr &member);
        void setMembers(const GameObjectPtrList &members);
        Q_PROPERTY(GameObjectPtrList members READ members WRITE setMembers)

        virtual void send(const QString &message, int color = Silver) const;

    private:
        GameObjectPtr m_leader;
        GameObjectPtrList m_members;
};

#endif // GROUP_H
