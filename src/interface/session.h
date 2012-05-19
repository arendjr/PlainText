#ifndef SESSION_H
#define SESSION_H

#include <QMetaType>
#include <QObject>


class Player;
class CommandInterpreter;

class Session : public QObject {

    Q_OBJECT

    public:
        explicit Session(QObject *object);
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
        enum {
            SessionClosed = 0,
            AskingUserName,
            AskingUserNameConfirmation,
            AskingPassword,
            AskingSignupPassword,
            AskingSignupPasswordConfirmation,
            SignedIn,
            SignInAborted
        } m_signInStage;

        class SignUpData;
        SignUpData *m_signUpData;

        Player *m_player;

        CommandInterpreter *m_interpreter;

        void processSignIn(const QString &data);
};

Q_DECLARE_METATYPE(Session *)

#endif // SESSION_H
