#ifndef TESTCASE_H
#define TESTCASE_H

#include <QObject>


class Engine;

class TestCase : public QObject {

    Q_OBJECT

    private slots:
        virtual void initTestCase();

        virtual void cleanupTestCase();

    private:
        void createTestWorld();
        void destroyTestWorld();

        Engine *m_engine;
};

#endif // TESTCASE_H
