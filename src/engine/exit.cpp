#include "exit.h"

#include <QDebug>

#include "badgameobjectexception.h"


Exit::Exit() {
}

bool Exit::operator==(const Exit &other) const {

    return m_name == other.m_name &&
           m_destinationArea == other.m_destinationArea;
}

void Exit::setName(const QString &name) {

    m_name = name;
}

void Exit::setDestinationArea(const GameObjectPtr &destinationArea) {

    m_destinationArea = destinationArea;
}

void Exit::setHidden(bool hidden) {

    m_hidden = hidden;
}

void Exit::resolvePointer() {

    m_destinationArea.resolve();
}

QVariantList Exit::toVariantList() const {

    QVariantList list;
    list.append(m_name);
    list.append(m_destinationArea.toString());
    list.append(m_hidden);
    return list;
}

Exit Exit::fromVariantList(const QVariantList &list) {

    if (list.length() != 3) {
        throw BadGameObjectException(BadGameObjectException::InvalidExitFormat);
    }

    Exit exit;
    exit.setName(list[0].toString());
    exit.setDestinationArea(GameObjectPtr::fromString(list[1].toString()));
    exit.setHidden(list[2].toBool());
    return exit;
}
