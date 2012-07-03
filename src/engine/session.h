#ifndef SESSION_H
#define SESSION_H

#include <QMetaType>
#include <QObject>


class CommandInterpreter;
class Player;
class Realm;

class Session : public QObject {

    Q_OBJECT

    public:
        Session(Realm *realm, QObject *object);
        virtual ~Session();

        void open();

        bool authenticated() const { return m_signInStage == SignedIn; }

        Player *player() const { return m_player; }

    public slots:
        void onUserInput(QString data);

    signals:
        void write(const QString &data);

        void terminate();

    private:
        enum SignInStage {
            SessionClosed = 0,
            AskingUserName,
            AskingUserNameConfirmation,
            AskingPassword,
            AskingSignupPassword,
            AskingSignupPasswordConfirmation,
            AskingRace,
            AskingClass,
            AskingGender,
            AskingExtraStats,
            AskingSignupConfirmation,
            SignedIn,
            SignInAborted
        } m_signInStage;

        class SignUpData;
        SignUpData *m_signUpData;

        Realm *m_realm;
        Player *m_player;

        CommandInterpreter *m_interpreter;

        void processSignIn(const QString &data);

        void showColumns(const QStringList &items);
};

Q_DECLARE_METATYPE(Session *)

#endif // SESSION_H
