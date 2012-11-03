#include "deleteobjectevent.h"

#include "gameobject.h"


DeleteObjectEvent::DeleteObjectEvent(GameObject *object) :
    Event(),
    m_object(object) {
}

DeleteObjectEvent::~DeleteObjectEvent() {
}

void DeleteObjectEvent::process() {

    delete m_object;
}

QString DeleteObjectEvent::toString() const {

    return QString("Delete object %2:%3").arg(m_object->objectType().toString())
                                         .arg(m_object->id());
}
