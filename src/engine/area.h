#ifndef AREA_H
#define AREA_H

#include "exit.h"
#include "gameobject.h"
#include "gameobjectptr.h"


class Area : public GameObject {

    Q_OBJECT

    public:
        explicit Area(uint id, Options options = NoOptions);
        virtual ~Area();

        const QString &title() const { return m_title; }
        void setTitle(const QString &title);
        Q_PROPERTY(QString title READ title WRITE setTitle);

        const QString &description() const { return m_description; }
        void setDescription(const QString &description);
        Q_PROPERTY(QString description READ description WRITE setDescription);

        const ExitList &exits() const { return m_exits; }
        void addExit(const Exit &exit);
        void removeExit(const Exit &exit);
        void setExits(const ExitList &exits);
        Q_PROPERTY(ExitList exits READ exits WRITE setExits);

        const GameObjectPtrList &presentCharacters() const { return m_presentCharacters; }
        void addPresentCharacter(const GameObjectPtr &character);
        void removePresentCharacter(const GameObjectPtr &character);
        void setPresentCharacters(const GameObjectPtrList &presentCharacters);
        Q_PROPERTY(GameObjectPtrList presentCharacters READ presentCharacters WRITE setPresentCharacters STORED false);

    private:
        QString m_title;
        QString m_description;
        ExitList m_exits;
        GameObjectPtrList m_presentCharacters;
};

#endif // AREA_H
