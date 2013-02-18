#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QScriptEngine>

#include "metatyperegistry.h"


class GameObject;
class Player;
class Realm;

class Session : public QObject {

    Q_OBJECT

    public:
        enum SessionState {
            SessionClosed = 0,
            SigningIn,
            SignedIn
        };

        Session(Realm *realm, const QString &description, const QString &source, QObject *parent);
        virtual ~Session();

        void open();

        const QString &source() const { return m_source; }
        Q_PROPERTY(QString source READ source)

        SessionState sessionState() const { return m_sessionState; }
        bool authenticated() const { return m_sessionState == SignedIn; }
        Q_INVOKABLE void setSessionState(int sessionState);

        void processSignIn(const QString &data);

        void signOut();

        Player *player() const { return m_player; }
        Q_INVOKABLE void setPlayer(GameObject *player);

        Q_INVOKABLE void send(const QString &message);

        static QScriptValue toScriptValue(QScriptEngine *engine, Session *const&session);
        static void fromScriptValue(const QScriptValue &object, Session *&session);

    public slots:
        void onUserInput(QString data);

    signals:
        void write(const QString &data);

        void terminate();

    private:
        QString m_source;

        SessionState m_sessionState;

        Realm *m_realm;
        Player *m_player;

        QScriptValue m_scriptObject;
};

PT_DECLARE_METATYPE(Session *)

#endif // SESSION_H
