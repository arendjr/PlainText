#include "signinevent.h"

#include <QDebug>

#include "session.h"


SignInEvent::SignInEvent(Session *session, const QString &input) :
    Event(),
    m_session(session),
    m_input(input) {
}

SignInEvent::~SignInEvent() {
}

void SignInEvent::process() {

    if (!m_session || m_input.isEmpty()) {
        qWarning() << "Processing uninitialized sign-in event. Skipped.";
        return;
    }

    m_session->processSignIn(m_input);
}

QString SignInEvent::toString() const {

    return QString("Sign-in input in state %1: %2").arg(m_session->sessionState()).arg(m_input);
}
