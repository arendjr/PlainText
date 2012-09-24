#include "asyncreplyevent.h"

#include <QDebug>

#include "player.h"


AsyncReplyEvent::AsyncReplyEvent(Player *recipient, const QString &reply) :
    Event(),
    m_recipient(recipient),
    m_reply(reply) {
}

AsyncReplyEvent::~AsyncReplyEvent() {
}

void AsyncReplyEvent::process() {

    if (!m_recipient) {
        qWarning() << "Processing uninitialized async reply event. Skipped.";
        return;
    }

    m_recipient->send(m_reply);
}

QString AsyncReplyEvent::toString() const {

    return "Async Reply: " + m_reply;
}
