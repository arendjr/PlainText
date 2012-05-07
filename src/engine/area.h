#ifndef AREA_H
#define AREA_H

#include "gameobject.h"
#include "gameobjectptr.h"


class Area : public GameObject {

    Q_OBJECT

    public:
        explicit Area(uint id, Options options = NoOptions);
        virtual ~Area();

        const GameObjectPtrList &exits() const { return m_exits; }
        virtual void addExit(const GameObjectPtr &exit);
        virtual void removeExit(const GameObjectPtr &exit);
        virtual void setExits(const GameObjectPtrList &exits);
        Q_PROPERTY(GameObjectPtrList exits READ exits WRITE setExits);

        const GameObjectPtrList &presentCharacters() const { return m_presentCharacters; }
        virtual void addPresentCharacter(const GameObjectPtr &character);
        virtual void removePresentCharacter(const GameObjectPtr &character);
        virtual void setPresentCharacters(const GameObjectPtrList &presentCharacters);
        Q_PROPERTY(GameObjectPtrList presentCharacters READ presentCharacters WRITE setPresentCharacters STORED false);

    private:
        GameObjectPtrList m_exits;
        GameObjectPtrList m_presentCharacters;
};

#endif // AREA_H
