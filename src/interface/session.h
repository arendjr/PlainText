#ifndef SESSION_H
#define SESSION_H

#include <QMetaType>
#include <QObject>


class Character;
class CommandInterpreter;

class Session : public QObject {

    Q_OBJECT

    public:
        explicit Session(QObject *object);
        virtual ~Session();

        void open();

        bool authenticated() const { return m_signInStage == SignedIn; }

        Character *character() const { return m_character; }

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

        Character *m_character;

        CommandInterpreter *m_interpreter;

        void processSignIn(const QString &data);
};

Q_DECLARE_METATYPE(Session *)

#endif // SESSION_H
