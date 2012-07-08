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
        };

        Session(Realm *realm, QObject *object);
        virtual ~Session();

        void open();

        SignInStage signInStage() const { return m_signInStage; }
        bool authenticated() const { return m_signInStage == SignedIn; }

        Player *player() const { return m_player; }

        void processSignIn(const QString &data);

    public slots:
        void onUserInput(QString data);

    signals:
        void write(const QString &data);

        void terminate();

    private:
        SignInStage m_signInStage;

        class SignUpData;
        SignUpData *m_signUpData;

        Realm *m_realm;
        Player *m_player;

        CommandInterpreter *m_interpreter;

        void showColumns(const QStringList &items);
};

Q_DECLARE_METATYPE(Session *)

#endif // SESSION_H
