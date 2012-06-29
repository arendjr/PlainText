#ifndef APPLICATION_H
#define APPLICATION_H

#include <QCoreApplication>


class Application : public QCoreApplication {

    Q_OBJECT

    public:
        Application(int &argc, char **argv);
        virtual ~Application();

        virtual bool notify(QObject *receiver, QEvent *event);
};

#endif // APPLICATION_H
