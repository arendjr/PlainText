#include "item.h"


Item::Item(uint id, Options options) :
    GameObject("item", id, options),
    m_portable(false) {
}

Item::~Item() {
}

void Item::setPortable(bool portable) {

    if (m_portable != portable) {
        m_portable = portable;

        setModified();
    }
}
