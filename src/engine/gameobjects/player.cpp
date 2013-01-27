#include "player.h"

#include <QCryptographicHash>
#include <QDebug>

#include "realm.h"
#include "room.h"
#include "session.h"
#include "util.h"


#define super Character

Player::Player(Realm *realm, uint id, Options options) :
    super(realm, GameObjectType::Player, id, options),
    m_regenerationIntervalId(0),
    m_admin(false),
    m_session(0) {

    setIndefiniteArticle("");
}

Player::~Player() {

    if (~options() & Copy) {
        realm()->unregisterPlayer(this);
    }
}

void Player::setPasswordSalt(const QString &passwordSalt) {

    if (m_passwordSalt != passwordSalt) {
        m_passwordSalt = passwordSalt;

        setModified();
    }
}

void Player::setPasswordHash(const QString &passwordHash) {

    if (m_passwordHash != passwordHash) {
        m_passwordHash = passwordHash;

        setModified();
    }
}

void Player::setPassword(const QString &password) {

    m_passwordSalt = Util::randomString(8);
    QByteArray data = QString(m_passwordSalt + password).toUtf8();
    m_passwordHash = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toBase64();

    setModified();
}

bool Player::matchesPassword(const QString &password) const {

    QByteArray data = QString(m_passwordSalt + password).toUtf8();
    QString passwordHash = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toBase64();
    return m_passwordHash == passwordHash;
}

void Player::setAdmin(bool admin) {

    if (m_admin != admin) {
        m_admin = admin;

        setModified();
    }
}

void Player::setSession(Session *session) {

    m_session = session;

    if (m_session) {
        m_regenerationIntervalId = realm()->startInterval(this, 30000);

        enter(currentRoom());
    } else {
        realm()->stopInterval(m_regenerationIntervalId);
        m_regenerationIntervalId = 0;

        if (secondsStunned() > 0) {
            setLeaveOnActive(true);
        } else {
            leave(currentRoom());
        }
    }
}

bool Player::isOnline() const {

    return m_session != nullptr;
}

void Player::send(const QString &_message, int color) const {

    if (!m_session) {
        return;
    }

    QString message;
    if (_message.endsWith("\n") ||
        (_message.startsWith("{") && _message.endsWith("}"))) {
        message = _message;
    } else {
        message = _message + "\n";
    }

    if ((Color) color != Silver) {
        message = Util::colorize(message, (Color) color);
    }

    m_session->send(message);
}

void Player::quit() {

    if (m_session != nullptr) {
        if (secondsStunned() > 0) {
            send(QString("Please wait %1 seconds.").arg(secondsStunned()), Olive);
        } else {
            m_session->signOut();
            setSession(nullptr);
        }
    }
}

void Player::invokeTimer(int timerId) {

    if (timerId == m_regenerationIntervalId) {
        invokeScriptMethod("regenerate");
        send("");
    } else {
        super::invokeTimer(timerId);
    }
}

void Player::changeName(const QString &newName) {

    super::changeName(newName);

    if (~options() & Copy) {
        realm()->registerPlayer(this);
    }
}
