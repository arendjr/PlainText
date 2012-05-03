#ifndef SESSION_H
#define SESSION_H

#include <QObject>


class Character;
class CommandInterpreter;

class Session : public QObject {

    Q_OBJECT

    public:
        explicit Session(QObject *object);
        virtual ~Session();

        void open();

    public slots:
        void onUserInput(QString data);

    signals:
        void write(QString data);

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

        Character *m_character;

        CommandInterpreter *m_interpreter;

        void processSignIn(const QString &data);
};

#endif // SESSION_H
