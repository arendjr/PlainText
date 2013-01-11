#include "deleteobjectevent.h"

#include <QDebug>

#include "gameobject.h"
#include "realm.h"


DeleteObjectEvent::DeleteObjectEvent(uint objectId) :
    Event(),
    m_objectId(objectId) {
}

DeleteObjectEvent::~DeleteObjectEvent() {
}

void DeleteObjectEvent::process() {

    GameObject *object = Realm::instance()->getObject(GameObjectType::Unknown, m_objectId);
    if (object) {
        delete object;
    } else {
        qDebug() << "Attempt to delete non-existing (already deleted?) object";
    }
}

QString DeleteObjectEvent::toString() const {

    return QString("Delete #%1").arg(m_objectId);
}
