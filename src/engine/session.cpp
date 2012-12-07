#include "session.h"

#include <QDebug>
#include <QMap>

#include "commandevent.h"
#include "constants.h"
#include "gameexception.h"
#include "gameobjectptr.h"
#include "logutil.h"
#include "player.h"
#include "realm.h"
#include "scriptengine.h"
#include "signinevent.h"
#include "util.h"


Session::Session(Realm *realm, const QString &description, const QString &source, QObject *parent) :
    QObject(parent),
    m_source(source),
    m_sessionState(SessionClosed),
    m_realm(realm),
    m_player(nullptr) {

    LogUtil::logSessionEvent(m_source, QString("Session opened (%1)").arg(description));
}

Session::~Session() {

    LogUtil::logSessionEvent(m_source, "Session closed");

    if (m_player && m_player->session() == this) {
        LogUtil::logCommand(m_player->name(), "(signed out)");

        m_player->setSession(nullptr);
    }
}

void Session::open() {

    try {
        setSessionState(SigningIn);

        ScriptEngine *engine = ScriptEngine::instance();
        m_scriptObject = engine->evaluate("new SessionHandler()");
        QScriptValue setSession = m_scriptObject.property("setSession");
        QScriptValueList args;
        args.append(engine->toScriptValue(this));
        setSession.call(m_scriptObject, args);

        if (engine->hasUncaughtException()) {
            QScriptValue exception = engine->uncaughtException();
            qWarning() << "Script Exception in Session::open(): " << exception.toString();
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Session::open(): " << exception.what();
    }
}

void Session::setSessionState(int sessionState) {

    m_sessionState = (SessionState) sessionState;

    switch (m_sessionState) {
        case SessionClosed:
            emit terminate();
            break;

        case SigningIn: {
            break;
        }

        case SignedIn: {
            if (!m_player) {
                throw GameException(GameException::InvalidSignIn,
                                    "Player object should have been set");
            }

            m_player->setSession(this);
            break;
        }
    }
}

void Session::processSignIn(const QString &data) {

    try {
        switch (m_sessionState) {
            case SessionClosed:
                break;

            case SigningIn: {
                QScriptValue processSignIn = m_scriptObject.property("processSignIn");
                QScriptValueList args;
                args.append(data.trimmed());
                processSignIn.call(m_scriptObject, args);
                break;
            }

            case SignedIn: {
                CommandEvent event(m_player, data);
                event.process();
                break;
            }
        }

        ScriptEngine *engine = ScriptEngine::instance();
        if (engine->hasUncaughtException()) {
            QScriptValue exception = engine->uncaughtException();
            qWarning() << "Script Exception in Session::processSignIn(): " << exception.toString();
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Session::processSignIn(): " << exception.what();
    }
}

void Session::signOut() {

    setSessionState(SessionClosed);
}

void Session::setPlayer(GameObject *player) {

    m_player = qobject_cast<Player *>(player);
    if (!m_player) {
        throw GameException(GameException::InvalidGameObjectCast);
    }
}

void Session::send(const QString &message) {

    emit write(message);
}

void Session::onUserInput(QString data) {

    if (m_sessionState == SessionClosed) {
        qDebug() << "User input on closed session";
        return;
    }

    if (data.isEmpty()) {
        return;
    }

    if (!m_player || !m_player->isAdmin()) {
        data = data.left(160);
    }

    if (m_sessionState == SignedIn) {
        m_realm->enqueueEvent(new CommandEvent(m_player, data));
    } else {
        m_realm->enqueueEvent(new SignInEvent(this, data));
    }
}

QScriptValue Session::toScriptValue(QScriptEngine *engine, Session *const &session) {

    return engine->newQObject(session, QScriptEngine::QtOwnership,
                              QScriptEngine::ExcludeDeleteLater |
                              QScriptEngine::PreferExistingWrapperObject);
}

void Session::fromScriptValue(const QScriptValue &object, Session *&session) {

    session = qobject_cast<Session *>(object.toQObject());
    Q_ASSERT(session);
}
