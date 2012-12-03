#include "group.h"


#define super GameObject

Group::Group(Realm *realm, uint id, Options options) :
    super(realm, GameObjectType::Group, id, (Options) (options | DontSave)) {
}

Group::~Group() {
}

void Group::setLeader(const GameObjectPtr &leader) {

    if (m_leader != leader) {
        m_leader = leader;
    }
}

void Group::addMember(const GameObjectPtr &member) {

    if (!m_members.contains(member)) {
        m_members << member;
    }
}

void Group::removeMember(const GameObjectPtr &member) {

    m_members.removeOne(member);
}

void Group::setMembers(const GameObjectPtrList &members) {

    if (m_members != members) {
        m_members = members;
    }
}

void Group::send(const QString &message, int color) const {

    m_leader->send(message, color);
    for (const GameObjectPtr &member : m_members) {
        member->send(message, color);
    }
}
