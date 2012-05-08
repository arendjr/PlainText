#ifndef ITEM_H
#define ITEM_H

#include "gameobject.h"


class Item : public GameObject {

    Q_OBJECT

    public:
        explicit Item(uint id, Options options = NoOptions);
        virtual ~Item();

        bool isPortable() const { return m_portable; }
        virtual void setPortable(bool portable);
        Q_PROPERTY(bool portable READ isPortable WRITE setPortable);

    private:
        bool m_portable;
};

#endif // ITEM_H
