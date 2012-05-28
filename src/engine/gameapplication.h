#ifndef GAMEAPPLICATION_H
#define GAMEAPPLICATION_H

#include <QCoreApplication>


class GameApplication : public QCoreApplication {

    Q_OBJECT

    public:
        explicit GameApplication(int &argc, char **argv);
        virtual ~GameApplication();

        virtual bool notify(QObject *receiver, QEvent *event);
};

#endif // GAMEAPPLICATION_H
