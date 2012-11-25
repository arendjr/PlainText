#include <QCoreApplication>
#include <QScriptEngine>
#include <QString>

#include "metatyperegistry.h"
#include "portal.h"
#include "realm.h"


int main(int argc, char *argv[]) {

    QCoreApplication application(argc, argv);
    QScriptEngine jsEngine;
    MetaTypeRegistry::registerMetaTypes(&jsEngine);

    Realm realm;
    realm.init();

    for (const GameObjectPtr &portalPtr : realm.allObjects(GameObjectType::Portal)) {
        Portal *portal = portalPtr.cast<Portal *>();
        PortalFlags flags = portal->flags();
        if (flags & (PortalFlags::CanOpenFromSide1 | PortalFlags::CanOpenFromSide2)) {
            portal->setFlags(flags |
                             PortalFlags::CanHearThroughIfOpen |
                             PortalFlags::CanSeeThroughIfOpen |
                             PortalFlags::CanShootThroughIfOpen |
                             PortalFlags::CanPassThroughIfOpen);
        } else {
            portal->setFlags(flags |
                             PortalFlags::CanHearThrough |
                             PortalFlags::CanSeeThrough |
                             PortalFlags::CanShootThrough |
                             PortalFlags::CanPassThrough);
        }
    }

    for (const GameObjectPtr &portalPtr : realm.allObjects(GameObjectType::Portal)) {
        portalPtr->save();
    }

    return 0;
}
